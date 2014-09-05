#!/bin/bash

# Install drupal site for CRADA
# SYNTAX:  install-crada-site.sh <git username> <git password>
# REQUIRES: PHP, MySQL Client, drush
#
CWD=$(pwd)

GIT_USERNAME=$1
GIT_PASSWORD=$2

ORGANIZATION_NAME=CBIIT
GIT_REPOSITORY=CRADA.git

#MYSQL
#WEBSITE_ROOT=/var/www/html/crada
#WEBSITE_ROOT=/vagrant/html/autotest
WEBSITE_ROOT=/var/www/html/crada
MYSQL_USERNAME=crada
MYSQL_PASSWORD=crada
MYSQL_DRUPAL_DATABASE=drupal
DRUPAL_ADMIN_PASSWORD=admin

#Check parameters
if [ "$GIT_PASSWORD" == "" ]; then
	echo
	echo "SYNTAX:"
	echo "install-crada-site.sh <git username> <git password>"
	echo 
	exit 1
fi
if [[ $EUID -eq 0 ]]; then
	#If user is root install these componets
	#Otherwise just skip for now
	echo "Installing necessary php modules"
	sudo yum -y install php-dom php-gd php-pdo
fi
echo " GIT_USERNAME: $GIT_USERNAME"
echo " GIT_PASSWORD: $GIT_PASSWORD"

#Make directory
mkdir -p $WEBSITE_ROOT

# install core
TEMP_DIR=drupal-new
echo "Install latest drupal core in a /tmp/$TEMP_DIR"
rm -Rf /tmp/$TEMP_DIR
cd /tmp
drush -y dl drupal --destination=drupal-new
# get drupal version dir name
DRUPAL_CORE_DIR=`ls $TEMP_DIR`
drupal_dir=${DRUPAL_CORE_DIR##*/}
# move drupal core files to current directory
echo "Moving drupal core to $WEBSITE_ROOT"
rsync -r /tmp/$TEMP_DIR/$drupal_dir/ $WEBSITE_ROOT
# delete tempdir
rm -rf $tempdir

#Add database
echo "Creating Initial Drupal Database:"
echo "  MYSQL_DRUPAL_DATABASE: $MYSQL_DRUPAL_DATABASE"
echo "  MYSQL_USERNAME: $MYSQL_USERNAME"
echo "  MYSQL_PASSWORD: $MYSQL_PASSWORD"

cd $WEBSITE_ROOT
drush -y si --db-url=mysql://$MYSQL_USERNAME:$MYSQL_PASSWORD@127.0.0.1:3306/$MYSQL_DRUPAL_DATABASE --account-pass=$DRUPAL_ADMIN_PASSWORD

#Add Drupal Modules
#dl = download
echo "Downolading Drupal Modules"
drush -y dl ctools
drush -y dl devel
drush -y dl libraries
drush -y dl phpexcel
drush -y dl services
#en = enable
echo "Enabling Druapl Modules"
drush -y en ctools
drush -y en devel
drush -y en libraries
drush -y en phpexcel
drush -y en services


#add crada site

echo "Adding CRADA site to $WEBSITE/site"
#Move settings.php out of the default folder
git clone https://$GIT_USERNAME:$GIT_PASSWORD@github.com/$ORGANIZATION_NAME/$GIT_REPOSITORY sites

#IMPORT The two drupal and crada databases
#TODO
echo "Dropping Database $MYSQL_DRUPAL_DATABASE"
mysql -p$MYSQL_PASSWORD -u$MYSQL_USERNAME drupal --execute="drop database $MYSQL_DRUPAL_DATABASE;"
echo "Importing CRADA tables into $MYSQL_DRUPAL_DATABASE"
echo "Importing nci_crada_drupal_tables_only.sql"
mysql -p$MYSQL_PASSWORD -u$MYSQL_USERNAME $MYSQL_DRUPAL_DATABASE < $CWD/nci_crada_drupal_tables_only.sql
echo "Importing nci_crada_crada_tables_only.sql"
mysql -p$MYSQL_PASSWORD -u$MYSQL_USERNAME $MYSQL_DRUPAL_DATABASE < $CWD/nci_crada_crada_tables_only.sql

echo
echo -n "Drupal Modules Enabled = "
drush pml --status=enabled --pipe |wc -l
echo 
echo "CRADA Drupal Site Installation Complete in $WEBSITE_ROOT"
echo 
