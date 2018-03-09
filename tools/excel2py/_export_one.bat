@echo off&title 导出配置
set PATH_CLIENT="./config_client"
set PATH_SERVER="./config"
set PATH_CFG="%~1"
if %PATH_CFG%=="" (echo tips：本批处理可以拖入文件到批处理图标上运行
					echo 请拖入文件：
					set /p PATH_CFG=
					rem set PATH_CFG="%f%"
					)
echo %PATH_CFG%	
echo %PATH_CFG%
python _export.py  %PATH_CFG% %PATH_CLIENT% %PATH_SERVER%

pause