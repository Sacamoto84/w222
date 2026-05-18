#!/bin/bash
# Версия: 1.1
# Дата: 11.01.2022
# Этот bash-скрипт последовательного пакета программного обеспечения CMSIS:
#
# Предварительные требования:
# - закончите bash (для Windows: установите git для Windows)
# - 7z в пути (утилита zip-архивирования)
# например Ubuntu: sudo apt-get install p7zip-full p7zip-rar)
# - PackChk в пути с технологиями выполнения
# (см.CMSIS-Pack:CMSIS/Utilities/<os>/PackChk)
# - xmllint в пути (проверка схемыXML)
# например Ubuntu: sudo apt-get install libxml2-utils
# Windows: скачать с https://www.zlatkovic.com/pub/libxml/

############### EDIT BELOW ###############
# Расширить переменную среду Локальный путь
#
if [ `uname -s` = "Linux" ]
  then
  CMSIS_PACK_PATH="/home/$USER/.arm/Packs/ARM/CMSIS/5.7.0/"
  PATH_TO_ADD="$CMSIS_PACK_PATH/CMSIS/Utilities/Linux64/"
else
  CMSIS_PACK_PATH="/C/Users/$USER/AppData/Local/Arm/Packs/ARM/CMSIS/5.7.0"
  PATH_TO_ADD="/C/Program Files (x86)/7-Zip/:/C/Program Files/7-Zip/:$CMSIS_PACK_PATH/CMSIS/Utilities/Win32/:/C/xmllint/"
fi
[[ ":$PATH:" != *":$PATH_TO_ADD}:"* ]] && PATH="${PATH}:${PATH_TO_ADD}"
echo $PATH_TO_ADD appended to PATH
echo " "

# Каталог склада упаковки – пункт назначения
PACK_WAREHOUSE=./

# Временный каталог сборки пакета
PACK_BUILD=build/

# Укажите каталоги, включенные в пакет, относительно базового каталога.
# Все каталоги:
# PACK_DIRS=`ls -d */`
# Не включайте каталог сборки, если он локальный.
# PACK_DIRS=${PACK_DIRS//$PACK_BUILD/}
# PACK_DIRS=${PACK_DIRS//$PACK_WAREHOUSE/}

# альтернатива: укажите имена каталогов, которые будут добавлены в базовый каталог пакета.
PACK_DIRS="
  ../../src
  ../../libs
  ../../demos
"


# Укажите имена файлов, которые будут добавлены в базовый каталог пакета.
PACK_BASE_FILES="
  ../../LICENCE.txt
  ../../README.md
  ../../lvgl.h
  ../../lv_version.h
  ../../lvgl_private.h
  lv_conf_cmsis.h
  lv_cmsis_pack.txt
"

############ DO NOT EDIT BELOW ###########
echo Starting CMSIS-Pack Generation: `date`
# Проверка утилиты ZIP
ZIP=7z
type -a $ZIP
errorlevel=$?
if [ $errorlevel -gt 0 ]
  then
  echo "Error: No 7zip Utility found"
  echo "Action: Add 7zip to your path"
  echo " "
  exit
fi

# Проверка утилиты проверки пакета
PACKCHK=PackChk
type -a $PACKCHK
errorlevel=$?
if [ $errorlevel != 0 ]
  then
  echo "Error: No PackChk Utility found"
  echo "Action: Add PackChk to your path"
  echo "Hint: Included in CMSIS Pack:"
  echo "<pack_root_dir>/ARM/CMSIS/<version>/CMSIS/Utilities/<os>/"
  echo " "
  exit
fi
echo " "

# Проверка утилиты проверки синтаксиса XML
XMLLINT=xmllint
type -a $XMLLINT
errorlevel=$?
if [ $errorlevel != 0 ]
  then
  echo "Error: No xmllint found"
  echo "Action: Add xmllint to your path"
  echo " "
  exit
fi
echo " "

# Найдите файл описания пакета.
# проверьте, существует ли более одного файла pdsc
NUM_PDSCS=`ls -1 *.pdsc | wc -l`
PACK_DESCRIPTION_FILE=`ls *.pdsc`
if [ $NUM_PDSCS -lt 1 ]
  then
  echo "Error: No *.pdsc file found in current directory"
  echo " "
elif [ $NUM_PDSCS -gt 1 ]
  then
  echo "Error: Only one PDSC file allowed in directory structure:"
  echo "Found:"
  echo "$PACK_DESCRIPTION_FILE"
  echo "Action: Delete unused pdsc files"
  echo " "
  exit
fi

SAVEIFS=$IFS
IFS=.
set $PACK_DESCRIPTION_FILE
# Поставщик пакетов
PACK_VENDOR=$1
# Название пакета
PACK_NAME=$2
echo Generating Pack Version: for $PACK_VENDOR.$PACK_NAME
echo " "
IFS=$SAVEIFS

#если каталог $PACK_BUILD не существует, создайте его.
if [ ! -d $PACK_BUILD ]; then
  mkdir -p $PACK_BUILD
fi

mkdir -p ${PACK_BUILD}/examples
mkdir -p ${PACK_BUILD}/examples/porting



# каталоги
echo Adding directories to pack:
echo $PACK_DIRS
echo " "
for d in ${PACK_DIRS}
do
  cp -r "$d" ${PACK_BUILD}
done

# файлы для базового каталога
echo Adding files to pack:
echo $PACK_BASE_FILES
echo " "
for f in $PACK_BASE_FILES
do
  cp -f  "$f" $PACK_BUILD/
done

# Скопируйте файлы в базовый каталог сборки: $ PACK_BUILD
# pdsc является обязательным в базовом каталоге:
cp -f  ./$PACK_VENDOR.$PACK_NAME.pdsc ${PACK_BUILD}
cp -f ../../examples/porting/* ${PACK_BUILD}/examples/porting
cp -f ./lv_os_custom_c.txt ${PACK_BUILD}/src/osal/lv_os_custom.c
cp -f ./lv_os_custom_h.txt ${PACK_BUILD}/src/osal/lv_os_custom.h

mv "${PACK_BUILD}/lv_cmsis_pack.txt" "${PACK_BUILD}/lv_cmsis_pack.c"

# Запустите схемы проверки (только для Linux):
# sudo apt-get установить libxml2-utils

echo Running schema check for $PACK_VENDOR.$PACK_NAME.pdsc
$XMLLINT --noout --schema ${CMSIS_PACK_PATH}/CMSIS/Utilities/PACK.xsd $PACK_BUILD/$PACK_VENDOR.$PACK_NAME.pdsc
errorlevel=$?
if [ $errorlevel -ne 0 ]; then
  echo "build aborted: Schema check of $PACK_VENDOR.$PACK_NAME.pdsc against PACK.xsd failed"
  echo " "
  exit
fi

# Запустите пакет проверки и сгенерируйте файл PackName с маркой.
$PACKCHK $PACK_BUILD/$PACK_VENDOR.$PACK_NAME.pdsc -n PackName.txt -x M362
errorlevel=$?
if [ $errorlevel -ne 0 ]; then
  echo "build aborted: pack check failed"
  echo " "
  exit
fi

PACKNAME=`cat PackName.txt`
rm -rf PackName.txt

# эхо применить патчи...
# rm -rf $PACK_BUILD/демос/ lv_demos.h
# cp -f ./lv_demos.h$PACK_BUILD/демос/

echo delete files...
find $PACK_BUILD/demos/ -type f -name "*.png" -delete
find $PACK_BUILD/demos/ -type f -name "*.gif" -delete
find $PACK_BUILD/demos/ -type f -name "*.gif" -delete
find $PACK_BUILD/demos/ -type f -name "*.ttf" -delete
find $PACK_BUILD/demos/ -type f -name "*.otf" -delete
find $PACK_BUILD/demos/ -type f -name "*.jpg" -delete
find $PACK_BUILD/demos/ -type f -name "*.fnt" -delete

# Архивирование
# $ZIP a $PACKNAME
echo creating pack file $PACKNAME
#если каталог $PACK_WAREHOUSE не существует, создайте его
if [ ! -d $PACK_WAREHOUSE ]; then
  mkdir -p $PACK_WAREHOUSE
fi
pushd $PACK_WAREHOUSE
PACK_WAREHOUSE=`pwd`
popd
pushd $PACK_BUILD
"$ZIP" a $PACK_WAREHOUSE/$PACKNAME -tzip
popd
errorlevel=$?
if [ $errorlevel -ne 0 ]; then
  echo "build aborted: archiving failed"
  exit
fi

# cp -f ./$PACK_VENDOR.$PACK_NAME.pdsc ${PACK_WAREHOUSE}


echo "build of pack succeeded"
# Очистить
echo "cleaning up ..."

rm -rf $PACK_BUILD

echo " "

echo Completed CMSIS-Pack Generation: `date`