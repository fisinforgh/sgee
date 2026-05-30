# ==============================================================================
# SGEE SoftEdu - Stern-Gerlach Experiment Experience
# ==============================================================================
#
# Copyright (C) 2026
#
# Authors:
#   [1] Julián Salamanca*
#   [2] Diego Julián Rodríguez-Patarroyo**
#
#   [1] jasalamanca@udistrital.edu.co (profesor Universidad Distrital)
#   [2] djrodriguezp@udistrital.edu.co (profesor Universidad Distrital)
#
#  * Grupo de Física e Informática (FISINFOR)
#  ** Grupo de Laboratorio de Fuentes Alternas de Energía (LIFAE)
#  *,** Universidad Distrital Francisco José de Caldas (Bogotá, Colombia)
#  
# Web page: https://github.com/fisinforgh/sgee
#
# LICENSE: GNU General Public License v3.0 (GPLv3) or later
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# ==============================================================================

#! /bin/bash

#This script UNINSTALL SGEESoftEdu software

sep="/"
sus="_"
sdot="."
deskEXT="desktop"
cont=0


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

messageUNIS=$(echo "Welcome to the SGEE SoftEdu UNINSTALL wizard"
	      echo "You are about to uninstall  SGEE SoftEdu software"
	      echo "If you are completely sure, please clik on UNINSTALL"
	      )

echo "$messageUNIS" | (zenity --text-info --title="SGEE SoftEdu uninstall" --width=800 --height=400 --ok-label='UNINSTALL' --cancel-label='Exit')
if [ $? -eq 1 ]; then
    echo "Exit"
    exit 2
fi

(zenity --info --text="Please click CONTINUE to select \"Desktop\" Folder to UNISTALL SGEE SoftEdu desktop's icon" --title="SGEE SoftEdu UNINSTALL desktop icon" --ok-label='CONTINUE' --width=800 --height=400 )

dirDESK=$(zenity --file-selection   --directory --title="SGEE SoftEdu UNINSTALL choose a Desktop Folder" --width=800 --height=400)

cd
homeDIR=$(pwd)
echo "Home folder: $homeDIR/"
echo " "

hiddenFILE=".pathDIR_SGEESoftEdu.txt"
dirNAME="SGEESoftEdu"
tarFILE="SGEESoftEdu.tar"
pathINSTALL=$(echo "$homeDIR$sep$hiddenFILE")

echo $pathINSTALL

if [ -f $pathINSTALL ]; then
    read -r line < $pathINSTALL
    rm $pathINSTALL
else
    manUNIS=$( echo "Remove SGEE SoftEdu software manually."
	       echo ""
	       echo "1. Find installation folder and remove:"
	       echo "  $dirNAME and $tarFILE"
	       echo ""
	       echo "2. Go to the desktop folder and remove:"
	       echo "  $dirNAME.desktop"
	       echo ""
	       echo "3. Go to hidden file \".bash_aliases\" and remove this two lines:"
	       echo "   # SGEESoftEdu globally available executable "
	       echo "   export PATH=\"some_folder_to$sep SGEESoftEdu:\$PATH"
	       echo ""
	       echo "4. Check if .pathDIR_SGEESoftEdu.txt exist, the erase it.")
    
    (zenity --info --text="$manUNIS" --width=800 --height=400  --ok-label='Close' )
    exit 2
fi
  
echo "0. $line"

cd $line

cd ..

installDIR=$(pwd)

echo "$installDIR"

if [ -d $installDIR/$dirNAME ]; then
    echo "1 rm $installDIR/$dirNAME"
    rm -r $installDIR/$dirNAME
else
    (zenity --info --text="$manUNIS" --width=800 --height=400  --ok-label='Close' )
    exit 2
fi


if [ -f $installDIR/$tarFILE ]; then
    echo "2 rm $installDIR/$tarFILE"
    rm $installDIR/$tarFILE
else
    (zenity --info --text="$manUNIS" --width=800 --height=400  --ok-label='Close' )
    exit 2
fi

echo "$dirDESK"
if [ -f $dirDESK/$dirNAME.desktop ]; then
    echo "3 rm $dirDESK/$dirNAME.desktop"
    rm $dirDESK/$dirNAME.desktop
else
    (zenity --info --text="$manUNIS" --width=800 --height=400  --ok-label='Close' )
    exit 2
fi

if [ -f $homeDIR/.bash_aliases ]; then
    sed -i "/$dirNAME/d" $homeDIR/.bash_aliases
    (zenity --info --text="UNINSTALL successesfully completed" --width=800 --height=400  --ok-label='Close' )
    exit 0
else
    (zenity --info --text="$manUNIS" --width=800 --height=400  --ok-label='Close' )
    exit 2
fi
