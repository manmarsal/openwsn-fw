import os
import sys
here = sys.path[0]
print here
sys.path.insert(0,os.path.join(here,'..','..','..','..','..','..','coap'))

from coap import coap

MOTE_IP = 'bbbb::1415:92cc:0:2'


c = coap.coap()

# read status of debug LED
p = c.GET('coap://[{0}]/temp_manel'.format(MOTE_IP))
i1 = int(p[0])
i1 = i1-48
i2 = int(p[1])
i2 = i2-48

temp = int('%i%i' % (i1,i2))
print (temp)
#print chr(p[])
#print chr(p[0])


# toggle debug LED
#p = c.PUT(
#    'coap://[{0}]/l'.format(MOTE_IP),
#    payload = [ord('2')],
#)

# read status of debug LED
#p = c.GET('coap://[{0}]/temp_manel'.format(MOTE_IP))
#i1 = int(p[0])
#i1 = i1-48
#i2 = int(p[1])
#i2 = i2-48

#temp = int('%i%i' % (i1,i2))
#print (temp)

os.system('kill %d' % os.getpid())

#raw_input("Done. Press enter to close.")
#exit()


