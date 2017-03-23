#!/usr/bin/python2
print 'Content-type: text/html\n'

from os.path import join, abspath
import cgi, sha, sys
Base_dir='/home/kiplove/tinyhttpd/htdocs/'
form=cgi.FieldStorage()
text=form.getvalue('text')
filename=form.getvalue('filename')
f=open(join(Base_dir,filename),'w')
f.write(text)
f.close()

print 'The file has been saved.'
