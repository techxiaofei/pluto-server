# -*- coding: utf-8 -*-
"""Alternative to reload().

This works by executing the module in a scratch namespace, and then
patching classes, methods and functions in place.  This avoids the
need to patch instances.  New objects are copied into the target
namespace.

Some of the many limitiations include:

- Global mutable objects other than classes are simply replaced, not patched

- Code using metaclasses is not handled correctly

- Code creating global singletons is not handled correctly

- Functions and methods using decorators (other than classmethod and
  staticmethod) is not handled correctly

- Renamings are not handled correctly

- Dependent modules are not reloaded

- When a dependent module contains 'from foo import bar', and
  reloading foo deletes foo.bar, the dependent module continues to use
  the old foo.bar object rather than failing

- Frozen modules and modules loaded from zip files aren't handled
  correctly

- Classes involving __slots__ are not handled correctly
"""

import imp
import sys
import types
import marshal
import inspect

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
		modns[name] = _update(tmpns[name], modns[name], ProcessGlobal, name)
	
	ProcessSubClasses(tmpns, modns, mod)
	# Done!
	return mod


def _update(oldobj, newobj, process_global=None, name=None):
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
	if type(oldobj) is not type(newobj) and not process_global:
		# Cop-out: if the type changed, give up
		return newobj
	if hasattr(newobj, "__reload_update__"):
		# Provide a hook for updating
		return newobj.__reload_update__(oldobj)
	if isinstance(newobj, types.ClassType) or hasattr(newobj, '__bases__'):
		return _update_class(oldobj, newobj)
	if isinstance(newobj, types.FunctionType):
		return _update_function(oldobj, newobj)
	if isinstance(newobj, types.MethodType):
		return _update_method(oldobj, newobj)
	if isinstance(newobj, classmethod):
		return _update_classmethod(oldobj, newobj)
	if isinstance(newobj, staticmethod):
		return _update_staticmethod(oldobj, newobj)
	if process_global:
		return process_global(oldobj, newobj, name)
	
	# Not something we recognize, just give up
	return newobj


# All of the following functions have the same signature as _update()


def _update_function(oldfunc, newfunc, update_cell_depth = 2):
	"""Update a function object."""
	oldfunc.__doc__ = newfunc.__doc__
	oldfunc.__dict__.update(newfunc.__dict__)
	oldfunc.__code__ = newfunc.__code__
	oldfunc.__defaults__ = newfunc.__defaults__
	
	if update_cell_depth and oldfunc.func_closure:
		for index, cell in enumerate( oldfunc.func_closure ):
			if inspect.isfunction( cell.cell_contents ):
				_update_function( cell.cell_contents, newfunc.func_closure[index].cell_contents, update_cell_depth - 1 )
	
	return oldfunc


def _update_method(oldmeth, newmeth):
	"""Update a method object."""
	# XXX What if im_func is not a function?
	_update(oldmeth.im_func, newmeth.im_func)
	#_update_function(oldmeth.im_func, newmeth.im_func)
	
	return oldmeth


def _update_class(oldclass, newclass):
	"""Update a class object."""
	olddict = oldclass.__dict__
	newdict = newclass.__dict__
	oldnames = set(olddict)
	newnames = set(newdict)
	for name in newnames - oldnames:
		setattr(oldclass, name, newdict[name])
	for name in oldnames - newnames:
		delattr(oldclass, name)
	for name in oldnames & newnames - {"__dict__", "__doc__"}:
		setattr(oldclass, name, _update(olddict[name], newdict[name]))
	return oldclass

def _update_classmethod(oldcm, newcm):
	"""Update a classmethod update."""
	# While we can't modify the classmethod object itself (it has no
	# mutable attributes), we *can* extract the underlying function
	# (by calling __get__(), which returns a method object) and update
	# it in-place.  We don't have the class available to pass to
	# __get__() but any object except None will do.
	_update(oldcm.__get__(0), newcm.__get__(0))
	return newcm


def _update_staticmethod(oldsm, newsm):
	"""Update a staticmethod update."""
	# While we can't modify the staticmethod object itself (it has no
	# mutable attributes), we *can* extract the underlying function
	# (by calling __get__(), which returns it) and update it in-place.
	# We don't have the class available to pass to __get__() but any
	# object except None will do.
	_update(oldsm.__get__(0), newsm.__get__(0))
	return newsm


def _restore_old_module(old_ns, new_ns):
	"""Restore the module to its previous state"""
	for _name, _value in old_ns.iteritems():
		new_ns[_name] = _value


def ProcessGlobal(old_value, new_value, name):
	"""if it's built-in object or all-upper-case constant or imported module, just replace it.
		But we should keep the those global objects unchanged since they may be our cache values in memory,
		and should not be replaced.
	"""
	
	if name.startswith("__"):
		print "=======1",name
		return new_value
	
	if name.isupper():
		print "=======2", name
		return new_value
	
	if isinstance(old_value, types.ModuleType):
		print "=======3", name
		return new_value
	print "=======4", name
	return old_value


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


