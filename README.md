INSTALLAIONT NOTES:

Step 1: Install drush
Execute install-drush.sh as sudo or as root user

sudo ./install-drush.sh

Step 2: Install CRATA site
Edit the install-crada-site.sh and make sure the MYSQL_USERNAME, MYSQL_PASSWORD, and WEBSITE_ROOT are set correctly. 

WEBSITE_ROOT=/var/www/html
MYSQL_USERNAME=crada
MYSQL_PASSWORD=crada

Execute the install-crada-site.sh script with proper github credentials with access to github.com/CBIIT/CRADA.

SYNTAX:
sudo ./install-crada-site.sh <github username> <github password>

When script completes you will have a brand new crada website with the latest code.


