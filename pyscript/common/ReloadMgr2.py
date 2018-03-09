# -*- coding: utf-8 -*-
__author = "majianfei"

import imp
import sys
import types
import marshal


def both_instance_of(first, second, klass):
	return isinstance(first, klass) and isinstance(second, klass)


def update_function(old_func, new_func):
	if not both_instance_of(old_func, new_func, types.FunctionType):
		return old_func
	if len(old_func.__code__.co_freevars) != len(new_func.__code__.co_freevars):
		return old_func
	old_func.__code__ = new_func.__code__
	old_func.__defaults__ = new_func.__defaults__
	old_func.__doc__ = new_func.__doc__
	old_func.__dict__ = new_func.__dict__
	if not old_func.__closure__ or not new_func.__closure__:
		return old_func
	for old_cell, new_cell in zip(old_func.__closure__, new_func.__closure__):
		if not both_instance_of(old_cell.cell_contents, new_cell.cell_contents, types.FunctionType):
			continue
		update_function(old_cell.cell_contents, new_cell.cell_contents)
	return old_func


def update_class(old_class, new_class):
	for name, new_attr in new_class.__dict__.items():
		if name not in old_class.__dict__:
			setattr(old_class, name, new_attr)
		else:
			old_attr = old_class.__dict__[name]
			if both_instance_of(old_attr, new_attr, types.FunctionType):
				update_function(old_attr, new_attr)
			elif both_instance_of(old_attr, new_attr, staticmethod):
				update_function(old_attr.__func__, new_attr.__func__)
			elif both_instance_of(old_attr, new_attr, classmethod):
				update_function(old_attr.__func__, new_attr.__func__)
			elif both_instance_of(old_attr, new_attr, property):
				update_function(old_attr.fdel, new_attr.fdel)
				update_function(old_attr.fget, new_attr.fget)
				update_function(old_attr.fset, new_attr.fset)
			elif both_instance_of(old_attr, new_attr, (type, types.ClassType)):
				update_class(old_attr, new_attr)
	return old_class


def update(oldobj, newobj, name):
	"""Update oldobj, if possible in place, with newobj.

	If oldobj is immutable, this simply returns newobj.

	Args:
	  oldobj: the object to be updated
	  newobj: the object used as the source for the update

	Returns:
	  either oldobj, updated in place, or newobj.
	"""
	
	if oldobj is newobj:
		# Probably something imported
		return newobj
	# if type(oldobj) is not type(newobj):
	# Cop-out: if the type changed, give up
	#	return newobj
	if hasattr(newobj, "__reload_update__"):
		# Provide a hook for updating
		return newobj.__reload_update__(oldobj)
	if isinstance(newobj, types.ClassType) or hasattr(newobj, '__bases__'):
		return update_class(oldobj, newobj)
	if isinstance(newobj, types.FunctionType):
		return update_function(oldobj, newobj)
	# if isinstance(newobj, types.MethodType):
	# 	return _update_method(oldobj, newobj)
	# if isinstance(newobj, classmethod):
	# 	return _update_classmethod(oldobj, newobj)
	# if isinstance(newobj, staticmethod):
	# 	return _update_staticmethod(oldobj, newobj)
	if name.startswith("__"):
		print "=======1", name
		return newobj
	
	if name.isupper():
		print "=======2", name
		return newobj
	
	if isinstance(oldobj, types.ModuleType):
		print "=======3", name
		return newobj
	print "=======4", name
	return oldobj
	
	# Not something we recognize, just give up
	return newobj


def xreload(modname):
	"""Reload a module in place, updating classes, methods and functions.

	Args:
	  mod: a module object

	Returns:
	  The (updated) input object itself.
	"""
	# Get the module name, e.g. 'foo.bar.whatever'
	mod = sys.modules.get(modname)
	if not mod:
		return
	
	# Get the module namespace (dict) early; this is part of the type check
	modns = mod.__dict__
	# Parse it into package name and module name, e.g. 'foo.bar' and 'whatever'
	i = modname.rfind(".")
	if i >= 0:
		pkgname, modname = modname[:i], modname[i + 1:]
	else:
		pkgname = None
	# Compute the search path
	if pkgname:
		# We're not reloading the package, only the module in it
		pkg = sys.modules[pkgname]
		path = pkg.__path__  # Search inside the package
	else:
		# Search the top-level module path
		pkg = None
		path = None  # Make find_module() uses the default search path
	# Find the module; may raise ImportError
	(stream, filename, (suffix, mode, kind)) = imp.find_module(modname, path)
	# Turn it into a code object
	try:
		# Is it Python source code or byte code read from a file?
		if kind not in (imp.PY_COMPILED, imp.PY_SOURCE):
			# Fall back to built-in reload()
			return reload(mod)
		if kind == imp.PY_SOURCE:
			source = stream.read()
			code = compile(source, filename, "exec")
		else:
			# Skip magic bytes and create time bytes, and load Code Object
			magic = imp.get_magic()
			create_time_bytes = 4
			stream.read(len(magic) + create_time_bytes)
			code = marshal.load(stream)
	finally:
		if stream:
			stream.close()
	# Execute the code.  We copy the module dict to a temporary; then
	# clear the module dict; then execute the new code in the module
	# dict; then swap things back and around.  This trick (due to
	# Glyph Lefkowitz) ensures that the (readonly) __globals__
	# attribute of methods and functions is set to the correct dict
	# object.
	tmpns = modns.copy()
	modns.clear()
	modns["__name__"] = tmpns["__name__"]
	try:
		exec (code, modns)
	except Exception, msg:
		_restore_old_module(tmpns, modns)
		return mod
	
	# Now we get to the hard part
	oldnames = set(tmpns)
	newnames = set(modns)
	# Update attributes in place
	for name in oldnames & newnames:
		modns[name] = update(tmpns[name], modns[name], name)
	
	ProcessSubClasses(tmpns, modns, mod)
	# Done!
	return mod

def _restore_old_module(old_ns, new_ns):
	"""Restore the module to its previous state"""
	for _name, _value in old_ns.iteritems():
		new_ns[_name] = _value


def ProcessSubClasses(old_ns, new_ns, mod):
	old_names = set(old_ns)
	new_names = set(new_ns)
	
	for _name in new_names - old_names:
		
		_obj = new_ns[_name]
		if not hasattr(_obj, '__bases__'):
			continue
		
		_new_bases = []
		for _base in _obj.__bases__:
			
			# If base class is in the same module with its subclass and is not a new class,
			# replace it with the old one in subclass.__bases__
			if _base.__module__ == mod.__name__ and old_ns.get(_base.__name__):
				_new_bases.append(old_ns[_base.__name__])
			
			else:
				_new_bases.append(_base)
		
		_obj.__bases__ = tuple(_new_bases)
