#!/bin/bash
ARCH_SUPPORTED=($(ls arch))

function create_board {
	echo "Creating board in: arch/${1}/${2}";
	board_path=arch/${1}/${2}/
	board_template_path=arch/template/board_template/
	mkdir -p ${board_path}

	# Create board headers from template
	cp -R ${board_template_path}/ ${board_path}/

	echo "Creating config entry in: config/${2}";
	config_path=config/${2}/
	config_board_template_path=config/template/
	mkdir -p ${config_path}
	cp -R ${config_board_template_path} ${config_path}
	cd ${config_path}/release/ && cat defconfig | sed "s/template/${2}/g" > .tmp.defconfig && mv .tmp.defconfig defconfig && cd -

	echo "Done !"
        echo "You can run: make config MACHINE_TYPE=${2}"
	exit
}

function read_arch_name {
	echo "what arch is the board ["${ARCH_SUPPORTED[*]}"] ?"
	read answer
	IS_ARCH_NEW=1
	for arch in ${ARCH_SUPPORTED[*]}; do
		if [ "$answer" == "$arch" ]; then
			echo "Selected ARCH is ${arch}"
			IS_ARCH_NEW=0
			create_board ${arch} ${1}
			exit
		fi
	done

	if [ $IS_ARCH_NEW == 1 ]; then
		echo "ARCH ${answer} is new. Confirm adding it [Y/y/n] ?"
		read confirm_arch_add
		if [ "$confirm_arch_add" != "${confirm_arch_add#[Yy]}" ]; then
			create_board ${answer} ${1}
		fi
	fi
}

function read_board_name {
	echo "what name has the board ?"
	read answer
	echo "Confirm board name ${answer} [Y/y/n] ?"
	read confirm_board_name
	if [ "$confirm_board_name" != "${confirm_board_name#[Yy]}" ]; then
		read_arch_name ${answer};
	fi
}

echo "do you wish to create a new board [Y/y/n] ?"
read answer
if [ "$answer" != "${answer#[Yy]}" ]; then
	echo "Yes"; read_board_name;
else
	echo "Bye !"; exit;
fi
