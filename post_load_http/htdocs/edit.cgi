#!/usr/bin/python2

print 'Content-type: text/html\n'


from os.path import join, abspath,exists,isfile
import cgi

form=cgi.FieldStorage()
Base_dir='/home/kiplove/tinyhttpd/htdocs'

filename=form.getvalue('filename')
filePath=join(Base_dir,filename)
text=open(filePath).read()
print """
 <html>
 <head>
 <title>Editing...</title>
 <meta charset="UTF-8" />
 </head>
 <body>
 <form action='save.cgi' method='POST'>
 <b>File:</b>%s<br/>
 <input type='hidden' value='%s' name='filename'/>
 <textarea name='text' cols='80' rows='20'>%s</textarea><br/>
 <input type='submit' value='Save' />
 </form>
 </body>
 </html>
 """ % (filename, filename, text)

