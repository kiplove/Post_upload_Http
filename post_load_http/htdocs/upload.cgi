#!/usr/bin/python2
# _*_ coding: utf-8 _*_


print ('Content-type: text/html\n')
import chardet
import os,sys
import re

Basedir='/home/kiplove/tinyhttpd/htdocs/temp/'

text=sys.stdin.read(int(os.environ["CONTENT_LENGTH"]))

line1='filename="?(\w+\.\w+)"?'
matchfilename=re.search(line1,text)
filename=matchfilename.group(1)

line2='(Content-Type: \w+\/\w+)(.+?)(-{20,30}\d{10,20})'
matchfile=re.search(line2,text,re.DOTALL)
filebuf=matchfile.group(2)

f=open(Basedir+filename,'w')
for char in filebuf:
	f.write(char)
f.close()


print (" <html> \
<head> \
<meta charset=\"UTF-8\" />\
</head>\
<body>\
<b>FILE:</b>%s<br/>\
<b>FILENAME:</b>%s<br/>\
</body>\
</html>\
" % (filebuf,filename))


