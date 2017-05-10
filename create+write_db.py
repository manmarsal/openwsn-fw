#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb as mdb
import sys
import time



hora = time.strftime("%H:%M:%S") #Formato de 24 horas
fecha = time.strftime("%y/%m/%d")

print hora
print fecha



try:
    con = mdb.connect('localhost', 'root', 'raspberry', 'edge_db');
    cur = con.cursor()
    #cur.execute("DROP TABLE IF EXISTS historico")
    # cur.execute("CREATE TABLE Writers(Id INT PRIMARY KEY AUTO_INCREMENT, \
    #             Name VARCHAR(25))")
    
    #sensor, dia, hora, valor_temp, valor humedad, calefaccion, motion
    
    cur.execute("INSERT INTO historico(valor_temp) VALUES('23')")
    cur.execute("INSERT INTO historico VALUES('03', '2017-05-10', '23:10:05', '23', '79', '0', '1')")
    
    # Guardar los cambios.
    con.commit()
    
except mdb.Error, e:
  
    print "Error %d: %s" % (e.args[0],e.args[1])
    sys.exit(1)
    
finally:    
        
    if con:    
        con.close()
