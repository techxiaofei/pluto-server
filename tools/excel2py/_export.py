# encoding: utf-8
__author__ = 'majianfei'
import sys
reload(sys)
sys.setdefaultencoding( "utf-8" )

import os
import time
import xlrd

FORMAT_TO_DICT = True
class ExcelReader(object):
	def __init__(self):
		pass

class ExcelSheetDataParser(object):
	def __init__(self, client_dir, server_dir, excel_file_name):
		self.out_field_index_names = ['server', 'client']
		self.max_data_cell_index = 0
		self.data_file_name = None  #导出的数据文件名称
		self.class_file_name = None	#导出的类名
		self.field_names = None     #导出的字段名称列表
		self.field_types = None     #数据类型
		self.field_indexs = {}      #导出的字段索引列表
		self.field_values = None    #导出的数值列表
		self.class_def = None		#类的描述，如字段名是哪列，值是哪列，注释是哪列
		self.out_put_type = None
		self.is_class_format = False
		self.excel_file_name = excel_file_name

		# 客户端服务端代码路径
		self.client_dir = client_dir
		self.server_dir = server_dir

		# 保存路径，不设置就是默认的tables路径
		self.data_save_dir = None
		self.class_save_dir = None

	def parse(self, book_sheet):
		self.field_values = []
		# print('...parse sheet:%s'%book_sheet.name)
		for row in xrange(book_sheet.nrows):
			first_cell = book_sheet.cell(row,0)
			if first_cell is None or first_cell.ctype != 1:
				continue

			key =first_cell.value.lower()

			if key == 'file':                                               #输出的文件名称
				self.data_file_name = book_sheet.cell(row, 1).value
				self.class_file_name = book_sheet.cell(row, 2).value
			if key == 'path':  # 输出到哪个目录
				for column, save_dir in enumerate([self.data_save_dir, self.class_save_dir]):
					path_str = book_sheet.cell(row, column+1).value
					if len(path_str) > 0:
						paths = path_str.split(',')
						if len(paths) > 1:
							client_dir = os.path.join(self.client_dir, paths[0]) if len(paths[0]) > 0 else None
							server_dir = os.path.join(self.server_dir, paths[1]) if len(paths[1]) > 0 else None
							if column == 0:
								self.data_save_dir = [client_dir, server_dir]
							else:
								self.class_save_dir = [client_dir, server_dir]
			elif key in self.out_field_index_names:                         #导出列配置
				self.parse_field_indexs(key, book_sheet, row)
			elif key == 'fields':                                           #导出字段
				self.parse_field_names(book_sheet, row)
			elif key == 'datatype':
				self.parse_field_types(book_sheet, row)
			elif key == 'value':
				self.add_field_values(book_sheet, row)
			elif key in ['class', 'const']:
				self.out_put_type = key
				self.parse_class_def(book_sheet, row)
		if self.field_indexs is None or len(self.field_indexs) == 0:
			return


	def out_put(self):
		if self.field_indexs is None or len(self.field_indexs) == 0:
			#print('....no data!!!')
			return
		for key, indexs in self.field_indexs.iteritems():
			if indexs is None or len(indexs) == 0:
				continue

			default_save_path = self.client_dir + '/config/tables' if key == 'client' else self.server_dir + '/config/tables'

			str = self.format_python(indexs)
			if self.data_save_dir:
				default_save_path = self.data_save_dir[0] if key == 'client' else self.data_save_dir[1]
			self._o_file(str, default_save_path)

		for key in ('client', 'server'):
			if self.class_def:
				str = self.generate_class()
				if self.class_save_dir:
					default_save_path = self.class_save_dir[0] if key == 'client' else self.class_save_dir[1]
				self._o_file(str, default_save_path, self.class_file_name+".py")

	def _o_file(self, _content, path, file_name = None):
		if path is None:
			return
		if not os.path.isdir(path):
			os.makedirs(path)

		file_name =  file_name if file_name else ''.join([ '' if self.is_class_format else 'data_', self.data_file_name, '.py'])
		path = os.path.join(path, file_name)
		fp = open(path, 'w')
		fp.write(_content)
		fp.close()
		print 'ok file save on:', path
		# print( '....export data:/%s/%s'%(path,file_name))

	def _format_class(self, indexs):
		strs = ['# encoding: utf-8\n']
		strs.append('class ')
		strs.append(self.data_file_name)
		strs.append('(object):\n\n')
		row_flag = False
		def get_field_index(field_name):
			for key, val in self.field_names.items():
				if val == field_name:
					return key
			return -1

		idx_property = get_field_index('property')
		idx_datatype = get_field_index('datatype')
		idx_value = get_field_index('value')
		idx_description = get_field_index('description')
		for cells in self.field_values:
			_property = self.get_cell_value_str(idx_property, cells[idx_property],True, datatype='common')
			if not _property or len(_property) == 0:
				continue

			_datatype = self.get_cell_value_str(idx_datatype, cells[idx_datatype], True,datatype='common')
			if not _datatype or len(_datatype) == 0:
				continue

			_description = self.get_cell_value_str(idx_property, cells[idx_description],True, datatype='common')
			if _description and len(_description) > 0:
				strs.append('\t#')
				strs.append(_description)
				strs.append('\n')

			_value = self.get_cell_value_str(idx_value, cells[idx_value],False, datatype=_datatype)
			strs.append('\t')
			strs.append(_property)
			strs.append('=')
			strs.append(_value)
			strs.append('\n\n')
		strs.append('\n')
		return ''.join(strs)

	def _format_dict(self, indexs):
		strs = ['# encoding: utf-8\n']
		strs.append('data = ')
		if FORMAT_TO_DICT:
			strs.append('{')
		else:
			strs.append('(')
		strs.append('\n')

		row_flag = False
		for cells in self.field_values:
			if row_flag:
				strs.append(',\n')
			strs.append('\t')
			if FORMAT_TO_DICT:
				strs.append(self.get_cell_value_str(1, cells[1], True))
				strs.append(':')

			strs.append('{')
			item_flag = False
			for idx in indexs:
				# change by dengchenghui 支持不导出空的字段
				field_name = self.field_names[idx]
				cell_value = self.get_cell_value_str(idx, cells[idx])
				if cell_value:
					if item_flag :
						strs.append(', ')
					strs.append("'" + field_name + "':" + cell_value)
					item_flag = True

			strs.append('}')
			row_flag = True
		strs.append('\n')
		if FORMAT_TO_DICT:
			strs.append('}')
		else:
			strs.append(')')
		strs.append('\n')
		return ''.join(strs)

	def format_python(self, indexs):
		if self.is_class_format:
			return self._format_class(indexs)
		else:
			return self._format_dict(indexs)

	def generate_class(self):
		strs = ['# encoding: utf-8\n', '# 注意这个类是自动导表生成的，不要手动修改，参考：%s\n\n' % os.path.basename(self.excel_file_name)]
		if self.out_put_type == 'class':
			strs.append('class ')
			strs.append(self.class_file_name)
			strs.append('(object):\n\n')

		idx_property = self.class_def.get('attribute', None)
		idx_datatype = self.class_def.get('datatype', 'string')
		idx_value = self.class_def.get('value', None)
		idx_description = self.class_def.get('annotation', '')

		for cells in self.field_values:
			_property = self.get_cell_value_str(idx_property, cells[idx_property], True, datatype='common')
			if not _property or len(_property) == 0:
				continue

			_datatype = self.get_cell_value_str(idx_datatype, cells[idx_datatype], True, datatype='common')
			if not _datatype or len(_datatype) == 0:
				continue

			_description = self.get_cell_value_str(idx_property, cells[idx_description], True, datatype='common')
			if _description and len(_description) > 0:
				strs.append('\t#' if self.out_put_type == 'class' else '#')
				strs.append(_description)
				strs.append('\n')

			_value = self.get_cell_value_str(idx_value, cells[idx_value], False, datatype=_datatype)
			if self.out_put_type == 'class':
				strs.append('\t')
			strs.append(_property)
			strs.append('=')
			strs.append(_value)
			strs.append('\n\n')
		strs.append('\n')
		strs.append('# 注意这个类是自动导表生成的，不要手动修改，参考：%s\n\n' % os.path.basename(self.excel_file_name))
		return ''.join(strs)

	def get_cell_value_str(self, cell_index, cell_obj, is_key_format = False, datatype = None):
		def get_cell_str(cellobj):
			val_str = None
			try:
				if cellobj.ctype == 2:
					fv = float(cellobj.value)
					iv = int(cellobj.value)
					if abs(fv-iv) != 0:
						val_str = '%s'%cellobj.value
					else:
						if iv != 0:
							val_str = str(iv)
						else:
							val_str = ''
				else:
					val_str = '%s'%cellobj.value
			except ValueError:
				val_str = cellobj.value
			return val_str

		_type = datatype if datatype else self.field_types[cell_index]
		if _type == 'string':
			val_str = get_cell_str(cell_obj)
			if val_str == u'':
				return None
			ret = []
			if not is_key_format:
				pass
				#ret.append("u")
			ret.append("'")
			ret.append(val_str)  #.decode('utf-8')
			ret.append("'")
			return ''.join(ret)
		elif _type == 'int':
			val_str = cell_obj.value
			if val_str == u'':
				return None
			return str(int(val_str))
		elif _type == 'array':
			val_str = get_cell_str(cell_obj)
			if val_str == u'':
				return None
			return '[' + val_str + ']'
		elif _type == 'dict':
			val_str = get_cell_str(cell_obj)
			if val_str == u'':
				return None
			return '{' + val_str + '}'
		elif _type == 'time':
			val_str = get_cell_str(cell_obj)
			if val_str == u'':
				return None
			# 转换为时间戳
			timeArray = time.strptime(val_str, "%Y-%m-%d %H:%M:%S")
			timeStamp = int(time.mktime(timeArray))
			return str(timeStamp)

		val_str = str(cell_obj.value)
		if val_str == u'':
			return None
		return val_str

	def parse_out_name(self, book_sheet, row):
		"""
		获取导出配置名称
		:param book_sheet:
		:param row:
		:return:
		"""
		self.data_file_name = book_sheet.cell(row, 1).value

	def parse_field_indexs(self, index_type, book_sheet, row):
		"""
		平台导出设置
		:param index_type:
		:param book_sheet:
		:param row:
		:return:
		"""
		data_indexs = []
		for col in xrange(book_sheet.row_len(row)):
			val = book_sheet.cell_value(row, col)
			if val == 'yes':
				data_indexs.append(col)
		self.field_indexs[index_type] = data_indexs
		# print('parse_field_indexs:%s'%self.field_indexs)


	def parse_field_types(self, book_sheet, row):
		self.field_types = {}
		for col_index in xrange(1, book_sheet.row_len(row)):
			field_type = book_sheet.cell_value(row, col_index)
			if field_type is None or len(field_type) == 0:
				break
			self.field_types[col_index] = field_type
			if field_type == 'property':
				self.is_class_format = True

	def parse_class_def(self, book_sheet, row):
		self.class_def = {}
		for col_index in xrange(1, book_sheet.row_len(row)):
			cell_value = book_sheet.cell_value(row, col_index)
			if cell_value is None or len(cell_value) == 0:
				continue
			self.class_def[cell_value] = col_index

	def parse_field_names(self,book_sheet, row):
		"""
		字段名称
		:param book_sheet:
		:param row:
		:return:
		"""
		self.field_names = {}
		for col_index in xrange(1, book_sheet.row_len(row)):
			field_name = book_sheet.cell_value(row, col_index)
			if field_name is None or len(field_name) == 0:
				break
			self.field_names[col_index] = field_name
		self.max_data_cell_index = len(self.field_names) + 1

	def add_field_values(self, book_sheet, row):
		data = {}
		for col_index in xrange(1,self.max_data_cell_index):
			cell = book_sheet.cell(row, col_index)
			data[col_index] = cell
		self.field_values.append(data)


class excel_exportor(object):
	def __init__(self, in_dir, client_out_dir, server_out_dir):
		self._in_dir = in_dir
		self._out_dir = client_out_dir
		self._client_out_dir = client_out_dir
		self._server_out_dir = server_out_dir
		self._files_list = []

	def _xlsx_files(self):
		self._files_list = []

		def _ft(filename):
			try:
				return True if filename.split('.')[1] == 'xlsx' and not filename.startswith('~') else False
			except IndexError:
				return False

		# print('os.path.isfile(%s)=%s'%(self._in_dir, os.path.isfile(self._in_dir)))
		if os.path.isfile(self._in_dir) and self._in_dir.endswith('.xlsx'):
			self._files_list.append(self._in_dir)
			return

		for root,dirs,files in os.walk(self._in_dir):
			for file in files:
				if _ft(file):
					self._files_list.append(root + os.sep + file)

	def export(self):
		self._xlsx_files()
		#print(self._files_list)
		if not os.path.exists(self._out_dir):
			os.makedirs(self._out_dir)

		print('=================== start ===============')

		for xlsx_file in self._files_list:
			workbook = xlrd.open_workbook(xlsx_file)
			print('parse excel:  %s'%xlsx_file)
			for booksheet in workbook.sheets():
				parser = ExcelSheetDataParser(self._client_out_dir, self._server_out_dir, xlsx_file)
				parser.parse(booksheet)
				parser.out_put()
		print('=================== end ===============')

def main(argv):
	# print(argv)
	# cfg_dir = os.getcwd()
	# client_dir = os.path.join(cfg_dir, 'client')
	# server_dir = os.path.join(cfg_dir, 'server')
	cfg_dir = argv[1]
	# print(cfg_dir)
	client_dir = argv[2]
	server_dir = argv[3]
	ee =  excel_exportor(cfg_dir,client_dir,server_dir)
	ee.export()
	print '\n====================success=====================\n'

if __name__ == '__main__':
	main(sys.argv)
