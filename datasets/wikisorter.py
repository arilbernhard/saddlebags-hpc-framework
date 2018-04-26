import string

ending = "</doc>"
t_beg = 'title="'
t_end = '">'

filenames = []
for i in range(0,10):
    filenames.append("wiki_0"+str(i))
for i in range(10,12):
    filenames.append("wiki_"+str(i))



for c_one in string.ascii_uppercase:
    for c_two in string.ascii_uppercase:
        foldername = c_one+c_two
        print foldername

'''

for filename in filenames:
    with open ("wikioutput/AA/"+filename, "r") as myfile:
    #with open (filename, "r") as myfile:
        s=myfile.read().replace('\n', ' ')
        
        while s != "":
            try:
                my_string = s[:s.index(ending)]
                article_name = s[s.index(t_beg) + len(t_beg) : s.index(t_end)]
                article_name = article_name.replace(' ', '_')
                my_string = s[s.index(t_end) + len(t_end) : s.index(ending) - len(ending)]
                s = s[s.index(ending)+len(ending):]
                with open("wikidump/"+article_name, "w") as text_file:
                    for i in range(0, 1):
                        text_file.write(my_string)


            except ValueError:
                break


'''