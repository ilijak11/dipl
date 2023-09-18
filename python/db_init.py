import sqlite3



conn = sqlite3.connect('user.db')
curs = conn.cursor()

curs.execute('CREATE TABLE user(name, tag, used)')
curs.execute('CREATE TABLE log(time, name, tag)')

#curs.execute('DELETE FROM log')
conn.commit()
