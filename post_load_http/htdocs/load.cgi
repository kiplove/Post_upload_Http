#!/usr/bin/perl -Tw
#下载文件所在目录
$uploadfolder="/home/kiplove/tinyhttpd/htdocs/temp";

#获取所需下载文件名称
$buffer = $ENV{'QUERY_STRING'};
@pairs = split(/&/, $buffer);
foreach $pair (@pairs)
{
   ($name, $value) = split(/=/, $pair);
   $value =~ tr/+/ /;
   $value =~ s/%(..)/pack("C", hex($1))/eg;
   $FORM{$name} = $value;
}
$name = $FORM{filename};

#实现下载
print "Content-Type:application/octet-stream; name=$uploadfolder/$name\r\n";
print "Content-Disposition: attachment; filename=$name\r\n\n";
open( FILE, "<$uploadfolder/$name" );
while(read(FILE, $buffer, 100) )
{
   print("$buffer");
}

