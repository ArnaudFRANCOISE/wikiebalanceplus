import serial
import time
import mysql.connector
from datetime import datetime
import pytz

mydb = mysql.connector.connect(
	host="localhost",
	user= "julien",
	password="pswd",
	database="test_tuto"
	)

cursor = mydb.cursor()
cursor.execute("create table if not exists ppair (id int auto_increment primary key, num varchar(255), temps varchar(255))")
cursor.execute("create table if not exists iimpair (id int auto_increment primary key , num varchar(255) , temps varchar(255))")



ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate=115200,
)

xx=[]
i=0
with open ("/dev/ttyUSB0",'r') as r:
        while 1:
                x=r.readline()
		if x != '':
			if x[0] == "{":
				xx=list(x.split(" "))
				xx.pop(0)
				num=int(xx[0])
				if num%2 == 0:
					now=datetime.utcnow()
					now=now.replace(tzinfo=pytz.utc)
					now=now.astimezone(pytz.timezone("Europe/Paris"))
					current_time=now.strftime("%Y-%m%d %H:%M:%S.%f")[:-3]
					cursor.execute("insert into ppair (num,temps) values (%s, %s)",(num,current_time))
					print(num)
					print('votre numero  est pair')
				else:
					now=datetime.utcnow()
					now=now.replace(tzinfo=pytz.utc)
					now=now.astimezone(pytz.timezone("Europe/Paris"))
					current_time=now.strftime("%Y-%m%d %H:%M:%S.%f")[:-3]
					cursor.execute("insert into iimpair (num,temps) values  (%s, %s)",(num,current_time))
					print(num)
					print("votre numero est impair")

		mydb.commit()
