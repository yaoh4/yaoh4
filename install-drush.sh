#!/bin/bash

#
# Drush Install script
#
# Install Drush using PHP pear for installing and maintaining Drupal sites
# Requires: PHP or greater
# Requires to run as root
#

yum install php-pear subversion
pear channel-update pear.php.net
pear upgrade --force Console_Getopt Console_Table
pear channel-discover pear.drush.org
pear install drush/drush

which drush
drush

#
#  If any errors try removing the .drush dirctory in home directory   
#  rm -Rf ~/.drush 
#

