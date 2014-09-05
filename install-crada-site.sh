#!/bin/bash

# Install drupal site for CRADA
# SYNTAX:  install-crada-site.sh <git username> <git password>
# REQUIRES: PHP, MySQL Client, drush
#
echo "***************************************"
echo "Warning this script is not fully tested."
echo "***************************************"


GIT_USERNAME=$1
GIT_PASSWORD=$2

#WEBSITE_ROOT=/var/www/html
WEBSITE_ROOT=/tmp/autotest
ORGANIZATION_NAME=CBIIT
GIT_REPOSITORY=CRADA.git

#MYSQL
MYSQL_USERNAME=crada
MYSQL_PASSWORD=crada
MYSQL_DRUPAL_DATABASE=drupal
DRUPAL_ADMIN_PASSWORD=admin

#Check parameters
if [ -z ${GIT_PASSWORD+x} ]; then
	echo
	echo "SYNTAX:"
	echo "install-crada-site.sh <git username> <git password>"
	exit
	else echo "var is set to '$var'"
fi

#install requirements
sudo yum -y install php-dom php-gd php-pdo

# install core
cd /tmp
tempdir=`mktemp -d -t drupal-new`
drush dl drupal --destination=$tempdir
# get drupal version dir name
tempdir_contents=`ls $tempdir`
drupal_dir=${tempdir_contents##*/}
# move drupal core files to current directory
rsync -r $tempdir/$drupal_dir/ $WEBSITE_ROOT
# delete tempdir
rm -rf $tempdir

#Add database
cd $WEBSITE_ROOT
drush si --db-url=mysql://$MYSQL_USERNAME:$MYSQL_PASSWORD@127.0.0.1:3306/$MYSQL_DRUPAL_DATABASE --account-pass=$DRUPAL_ADMIN_PASSWORD

#Add Drupal Modules
#dl = download
drush dl ctools
drush dl devel
drush dl libraries
drush dl phpexcel
drush dl services
#en = enable
drush -y en ctools
drush -y en devel
drush -y en libraries
drush -y en phpexcel
drush -y en services


#add crada site
cd site
git clone https://$GIT_USERNAME:$GIT_PASSWORD@github.com/$ORGANIZATION_NAME/$GIT_REPOSITORY site

#IMPORT The two drupal and crada databases
#TODO
cd ..
mysql -p $MYSQL_PASSWORD -u $MYSQL_USERNAME $MYSQL_DRUPAL_DATABASE < nci_crada_drupal_tables_only.sql
mysql -p $MYSQL_PASSWORD -u $MYSQL_USERNAME $MYSQL_DRUPAL_DATABASE < nci_crada_drupal_tables_only.sql

echo
echo -n "Drupal Modules Enabled = "
drush pml --status=enabled --pipe |wc -l
echo 
echo "CRADA Drupal Site Installation Complete"
echo 
