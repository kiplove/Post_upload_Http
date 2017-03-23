#!/usr/bin/perl  
#文件下载目录
$show=`ls  /home/kiplove/tinyhttpd/htdocs/temp`;
$show=~ s/[\s]/         /g;
print "Content-type:text/html\r\n\r\n";
print "<html>";
print "<head>";
print '<meta charset="utf-8">';
print '<title>kiplove.cn</title>';
print "</head>";
print "<body>";
print $show;
print "<p>选择下载文件</p>";
print "<FORM METHOD=\"GET\" ACTION=\"load.cgi\">";
print "<input type=\"text\" name=\"filename\">";
print "<input type=\"submit\" value=\"load_file\">";
print "</FORM>";
print "</body>";
print "</html>";

