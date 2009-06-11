<?php
/**
 * @brief index.php
 * @author Alexis Moinet
 * @date 11/06/2009
 * @copyright (c) 2009 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
*/
?>

<?php
require_once 'config.php';
require_once 'setup.php';

?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title><?php $gOut->printContent('sitename'); ?> - <?php $gOut->printContent('pagename'); ?></title>
		<style type="text/css" media="screen,projection">/*<![CDATA[*/ @import "style/skin.css"; /*]]>*/</style>
		<style type="text/css">
			#rateStatus{float:left; clear:both; width:100%; height:20px;}
			#rateMe{float:left; clear:both; width:100%; height:auto; padding:0px; margin:0px;}
			#rateMe li{float:left;list-style:none;}
			#rateMe li a:hover,
			#rateMe .on{background:url(on.jpg) no-repeat;}
			#rateMe a{margin-left:1px; border: 1px solid black; float:left;background:url(off.jpg) no-repeat;width:12px; height:12px;}
			#ratingSaved{display:none;}
			.saved{color:red; }
		</style>

    </head>
    <body>
	<div id="main">
		<div id="header">header</div>
		<div class="clear"></div>
		<div id="topmenu">
			<div id="menu"><?php $gOut->printContent('menu');?></div>
			<div id="userlinks"><?php $gOut->printContent('userlinks');?></div>
		</div>
		<div class="clear"></div>
		<?php if ($gOut->getContent('error')) { ?>
		<div id="errormsg">
			<?php $gOut->printContent('errormsg'); ?>
		</div>
		<div class="clear"></div>
		<?php } ?>
		<div id="content">
			<div id="recorder"><?php $gOut->printContent('recorder');?></div>
			<div id="pagecontent"><?php	$gOut->printContent('pagecontent'); ?>
			</div>
			<div id="sidecontent">
			<?php $gOut->printContent('sidecontent');?>
			<?php $gOut->printContent('last-laugh');?>
			<?php $gOut->printContent('last-comments');?>
			</div>
		</div>
		<div class="clear"></div>
		<div id="footer">footer
		<?php echo $gOut->toHtml(); ?>
		</div>
	</div>
    </body>
</html>

<?php

?>