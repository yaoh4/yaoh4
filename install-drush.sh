#!/bin/bash

#
# Drush Install script
#
# Install Drush using PHP pear for installing and maintaining Drupal sites
# Requires: PHP or greater
# Requires to run as root
#

if [[ $EUID -gt 0 ]]; then
	echo "Run this script with sudo or as the root user"
	echo 
	echo "SYNTAX:"
	echo "sudo ./install-drush.sh"
	echo
    exit 1
fi

echo "Installing php-pear and subversion"
yum install php-pear subversion
echo "Setting channel-update pear.php.net"
pear channel-update pear.php.net
echo "upgrading required pear packages"
pear upgrade --force Console_Getopt Console_Table
echo "Setting channel-discover to pear.drush.org"
pear channel-discover pear.drush.org
echo "Installing drush"
pear install drush/drush

echo "Checking version "
/usr/bin/drush --version
echo 
echo "drush install completed"
echo 

#
#  If any errors try removing the .drush dirctory in home directory   
#  rm -Rf ~/.drush 
#

