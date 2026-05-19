###########################################################################
#                                                                         #
# NOTICE OF COPYRIGHT                                                     #
# Software Educativo Stern Gerlach Experiment Experience                  #
# SGEESoftEdu                                                             #
#                                                                         #
# Copyright (C) 2026                                                      #
#                                                                         #
# Authors:								  #
#   [1] Julian A Salamanca B, PhD					  #
#   [2] Diego J Rodriguez P				                  #
#                                                                         #
#  [1] jasalamanca@udistrital.edu.co (profesor Universidad Distrital)     #
#  [2] djrodriguezp@udistrital.edu.co (profesor Universidad Distrital)    #
#									  #
#  * Grupo de Investigación Física e Informática (FISINFOR)		  #
#  ** Grupo de Laboratorio de Fuentes Alternas de Energía (LIFAE)	  #
#  *,** Universidad Distrital Francisco José de Caldas (Bogotá, Colombia) #	
#                                                                         #
# This program is free software; you can redistribute it and/or modify    #
# it under the terms of the GNU General Public License as published by    #
# the Free Software Foundation; either version 2 of the License, or       #
# (at your option) any later version.                                     #
#                                                                         #
###########################################################################

#! /bin/bash

#This script installs SGEESoftEdu software natively
#It does not look at hidden files for root.exe:
#  find $homeDIR/ -not -path '*/.*' -iname $_ROOT

sep="/"
sus="_"
sdot="."
deskEXT="desktop"
cont=0

cd
homeDIR=$(pwd)
echo "Home folder: $homeDIR/"
echo " "

_ROOT="root.exe"

Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White
Color_Off='\033[0m'       # Text Reset
Bg_red='\033[0;41m'
    
if command -v $_ROOT > /dev/null ;
then
    echo -e "ROOT System-wide install found it! ${Green}\u2714 \u2714 ${Color_Off}"
    $_ROOT --version
    let "cont = 1"
    pathROOTEXE="$_ROOT"
    pathROOTCLING="rootcling"
    pathROOTCONFIG="root-config"
else
    echo "NO ROOT system-wide install found."
    echo "---> Looking for user's ROOT install at $homeDIR..."
    check=$(find $homeDIR/ -not -path '*/.*' -iname $_ROOT | cat | wc -l)
    if [ $check -ne 0 ];
    then
	echo " "
	echo -e "User's ROOT install found it!!${Green}\u2714 \u2714 ${Color_Off} $_ROOT at:"
	find $homeDIR/ -iname $_ROOT
	echo " "
	pathROOTEXE=$(find $homeDIR/ -not -path '*/.*' -iname $_ROOT)
	pathROOTCLING=$(find $homeDIR/ -not -path '*/.*' -iname rootcling)
	pathROOTCONFIG=$(find $homeDIR/ -not -path '*/.*' -iname root-config)
	let "cont = 2"
    else
	echo " "
	echo -e "${Red} \u274c ${Color_Off} ${Blue}$_ROOT${Color_Off} command ${Bg_red}NOT${Color_Off} found"
	echo " Please install ROOT-CERN "
	echo " https://root.cern/install/"
	echo " We recommend install precompiled version. Go to 'Download a pre-compiled binary distribution' section for further info."
	echo " "
	warnInfo=$(echo " "
		   echo -e "\u274c $_ROOT$ NOT$ found"
		   echo " Please install ROOT-CERN "
		   echo " https://root.cern/install/"
		   echo " We recommend install precompiled version. Go to 'Download a pre-compiled binary distribution' section for further info."
		   echo " ")
	zenity --info --text="$warnInfo" --ok-label="Exit install"
	echo "bye"
	sleep 1
	exit 1
    fi
fi

if [ $cont -eq 1 ]; then
    infoINS1=$(
	echo "ROOT System install found it!"
	echo "Please click CONTINUE to select SGEESoftEdu Installation Folder"
	echo " "
	command -v $_ROOT
	$pathROOTEXE -q
	echo "Executables found: "
	echo "rootcling, root-config:"
	echo "$pathROOTCLING"
	echo "$pathROOTCONFIG"
	echo " ")
    echo "$infoINS1" | (zenity --text-info --title="SGEE SoftEdu install" --width=800 --height=400 --ok-label='CONTINUE' --cancel-label='Exit install')
    if [ $? -eq 1 ]; then
	echo "Exit"
	exit 2
    fi   
else
    infoINS2=$(echo "Please click CONTINUE to select installation folder"
	       echo " "
	       echo "Info $_ROOT:"
	       echo "NO ROOT system-wide install found."
	       echo "---> Looking for user's ROOT install at $homeDIR..."
	       echo "User's ROOT install found it!!. $_ROOT at:"
	       echo "$pathROOTEXE"
	       $pathROOTEXE -q
	       echo "rootcling, root-config:"
	       echo "$pathROOTCLING"
	       echo "$pathROOTCONFIG"
	       echo "Please click CONTINUE to select installation folder"
	    )
    echo "$infoINS2" | (zenity --text-info --title="SGEE SoftEdu install" --width=800 --height=400 --ok-label='CONTINUE' --cancel-label='Exit install')
    if [ $? -eq 1 ]; then
	echo "Exit"
	exit 2
    fi   

fi

dirPATH=$(zenity --file-selection   --directory --title="SGEE SoftEdu install Choose a Installation Folder" --width=800 --height=400 )

(zenity --info --text="Please click CONTINUE to select Desktop Folder" --title="SGEE SoftEdu install choose Desktop Folder" --ok-label='CONTINUE' --width=800 --height=400 )

dirDESK=$(zenity --file-selection   --directory --title="SGEE SoftEdu install choose a Desktop Folder" --width=800 --height=400)

tarFILE="SGEESoftEdu.tar"
pwd
echo "Installation folder: $dirPATH"
echo "Path to Desktop folder: $dirDESK"
echo "Downloading SGEE source files: ... "
echo "wget https://github.com/fisinforgh/sgee/raw/refs/heads/main/$tarFILE -P $dirPATH"
echo "Wait..."
(wget -O $dirPATH/$tarFILE https://github.com/fisinforgh/sgee/raw/refs/heads/main/$tarFILE) | zenity --progress --width=800 --height=400 --auto-close \
										     --title="SGEESoftEdu_1.0 Install" \
										     --text="Downloading SGEESofEdu... Please wait ..." \
										     --percentage=10
echo " "

if [ -f $dirPATH/$tarFILE ];
then
    (echo "Please click CONTINUE to install SGEESoftEdu"
    echo "$tarFILE successfully downloaded!!") | (zenity --text-info --width=800 --height=400  --title="SGEE SoftEdu install" \
						       --ok-label='CONTINUE' --cancel-label='Exit install')
    if [ $? -eq 1 ]; then
	echo "Exit"
	exit 2
    fi    
else
    (echo "wget FAILED. It may possible web page not available"
     echo "Please check your internet connection and try it again"
    ) | (zenity --info --text="$warnInfo" --ok-label="Exit install")
    if [ $? -eq 1 ]; then
	echo "Exit"
	exit 2
    fi
fi


nameDIR="SGEESoftEdu"
FILE="SESGEE"

cd $dirPATH
tar xfv $tarFILE
cd $nameDIR

if [ -f "$FILE" ]; then
    rm $FILE
    ($pathROOTCLING -f SESGEEDict.cxx -c SESGEE.h SESGEELinkDef.h
     g++ -o SESGEE SESGEE.cxx SESGEEDict.cxx `$pathROOTCONFIG --cflags --glibs` -lGeom -lRGL -lGui -lCore -lMathCore) | zenity --progress --width=800 --height=400 --auto-close \
												    --title="SGEESoftEdu_1.0 Install" \
												    --text="Compiling SGEE SoftEdu_1.0 software" \
												    --percentage=10
else
    ($pathROOTCLING -f SESGEEDict.cxx -c SESGEE.h SESGEELinkDef.h
     g++ -o SESGEE SESGEE.cxx SESGEEDict.cxx `$pathROOTCONFIG --cflags --glibs` -lGeom -lRGL -lGui -lCore -lMathCore) | zenity --progress --width=800 --height=400 --auto-close \
												    --title="SGEESoftEdu_1.0 Install" \
												    --text="Compiling SGEE SofEdu_1.0 software" \
												    --percentage=10
fi


if [ -f "$FILE" ]; then
    echo "Executable compilation complete"
    (zenity --info --text="SGEESoftEdu_1.0 executable compilation complete $FILE. \nPlease click CONTINUE to setup SGEE executable globally and  desktop icon "\
	    --title="SGEE SoftEdu install" --ok-label='CONTINUE' --width=800 --height=400 )
else 
    echo "Something went WRONG. Please check requirements"
    echo "and try again"
    (zenity --info --text="Please check g++ compiler and try it again "\
	    --title="SGEE SoftEdu install ERROR" --ok-label='Exit install' --width=800 --height=400 )
    if [ $? -eq 0 ]; then
	echo "Exit"
	exit 2
    fi
fi

# Creating hidden path file..."

echo " "
echo "$dirPATH/$nameDIR" > ~/.pathDIR_$nameDIR.txt

if [ -f $homeDIR/.bash_aliases ]; then
    sed -i "/$nameDIR/d" $homeDIR/.bash_aliases
    echo "# $nameDIR globally available executable " >> $homeDIR/.bash_aliases
    echo "export PATH=\"$dirPATH/$nameDIR:\$PATH\"" >> $homeDIR/.bash_aliases
else
    echo "# $nameDIR globally available executable " >> $homeDIR/.bash_aliases
    echo "export PATH=\"$dirPATH/$nameDIR:\$PATH\"" >> $homeDIR/.bash_aliases
fi

deskFILE=$dirPATH/$nameDIR/$nameDIR$sdot$deskEXT

echo "Configuring desktop file..."

echo "$deskFILE"

echo "[Desktop Entry]" > "$deskFILE"
echo "Version=1.0" >> "$deskFILE"
echo "Type=Application" >> "$deskFILE"
echo "Exec=$dirPATH/$nameDIR/$FILE" >> "$deskFILE"
echo "Icon=$dirPATH/$nameDIR/icons/logoSGEE.png" >> "$deskFILE"
echo "Terminal=false" >> "$deskFILE"
echo "Name=$nameDIR" >> "$deskFILE"
echo "Comment=Stern-Gerlach Experiement Simulation">> "$deskFILE"
echo "MimeType=application/x-root;text/x-c++src;">> "$deskFILE"
echo "Categories=Science;">> "$deskFILE"

echo "Desktop file: ... "
cat $deskFILE
dirSRCDESK="desktop_file"
infoDESK_FILE=$(echo "Globally executable completed, hidden bash file:"
		echo "$homeDIR/.bash_aliases"
		echo "# SGEESoftEdu globally available executable "
		echo "export PATH=\"$dirPATH/$nameDIR:\$PATH\""
		echo "\n Desktop file configuration success: \n"; cat $deskFILE ; echo "\n SGEESofEdu Installation COMPLETED!! \n Please close and enjoy!!")

cp $deskFILE $dirDESK
mv $deskFILE $dirPATH/$nameDIR/$dirSRCDESK

(zenity --info --text="$infoDESK_FILE" --width=800 --height=400  --ok-label='Close' )
echo "Installation COMPLETED!!!"
exit 0
