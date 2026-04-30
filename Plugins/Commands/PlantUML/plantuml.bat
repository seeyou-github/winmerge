@echo off
setlocal EnableDelayedExpansion
set "PlantUMLJar=plantuml-1.2023.0.jar"
set "PlantUML_PATH=%~dp0%PlantUMLJar%"
if not exist "%PlantUML_PATH%" (
  echo PlantUML is not installed. Network downloads are disabled in this build. 1>&2
  exit /b 1
)
(
  (
    (if /i "%~x1" == ".json" echo @startjson)
    (if /i "%~x1" == ".yml" echo @startyaml)
    (if /i "%~x1" == ".yaml" echo @startyaml)
  )
  type "%~1"
  (
    (if /i "%~x1" == ".json" echo @endjson)
    (if /i "%~x1" == ".yml" echo @endyaml)
    (if /i "%~x1" == ".yaml" echo @endyaml)
  )
) | call "%~dp0..\Java\java.bat" -jar "%PlantUML_PATH%" -pipe %3 %4 %5 %6 %7 %8 %9 > "%~2"
