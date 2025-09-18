#!/usr/bin/env python3

import serial
import sys
import time
import struct
import numpy as np
import serial
import re
import csv
from datetime import datetime

import os
import pyqtgraph as pg
from PyQt5 import QtWidgets, QtCore


import logging
log = logging.getLogger( __name__ )


class IOnode():
    """  """
    def __init__(self, SerDevStr='/dev/ttyUSB0', ACKwaittime = 0.5  ):

        log.debug( "Enter __init__()" )

        retval = True

        self.dev = False

        try:
            self.dev = serial.Serial(
                port=SerDevStr,
                baudrate=115200,  # baudrate=230400 #baudrate=500000
                #baudrate=230400,
                #baudrate=500000,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                write_timeout = 0.01,
                timeout = 0.01 )

        except:
            msg = 'Could not open serial device'
            log.error( msg )
            sys.exit( msg )

        # clear buffers
        self.dev.reset_input_buffer()
        self.dev.reset_output_buffer()


        # read any data in buffer to clear
        d = self.dev.read( self.dev.in_waiting )


        ## send ID to node/board

        ID = bytearray( [ 0x66, 0xde, 0xad, 0xbe, 0xef, 0x22 ] )

        nw = self.dev.write( ID )
        if not nw == len( ID ):
            msg = 'Could not write ID signature to board - exiting'
            log.error( msg )
            retval = False
            

        # read ACK/NACK frame
        
        log.debug( 'Wait for NACK frame on sending ID frame to board' )

        typ, length, data = self.ReadFrame( TimeOut = ACKwaittime, Test = False )

        if typ == -1:

            log.error( 'Did not get ACK/NACK frame' )
            retval = False

        else:

            AckFrameType = data[0]
            AckNack = data[1]

            log.debug( 'Line setup frame Ack/Nack, Type: {}, Length: {}, Data: {}'.format( typ, length, data) )

            if not (AckFrameType == 0x02 and typ ==0x02 and length == 2 ):
                msg = 'Could not setup serial line: incorrect ACK frame format!'
                log.error( msg )
                retval = False


            if( AckNack == True ):
                log.info( 'Serial line setup sucess' )
                retval = True

            else:
                msg = 'Could not setup serial line'
                log.error( msg )
                retval = False


        if retval == False and not self.dev == 0:

            log.debug( 'Send Reset command to node to ensure state (if node is there)' )

            self.Reset()

            self.dev.close()
            self.dev=False



        
    def __del__(self):
        
        log.debug( "Enter __del__()" )

        if not self.dev == 0:

            self.Reset( ACKwaittime = 2)

            self.dev.close()
        else:
            log.debug( "Device not open, so not closing" )


    ####
    def ReadFrame(self, TimeOut = None, Test = False ):

        self.dev.timeout = TimeOut  # defaults to blocking!
        
        d = self.dev.read( 2 )
        
        if len(d) < 2:

            msg = 'Could not read 2 byte frame header, ({} read), Timeout: {}'.format( len(d), self.dev.timeout )
            if Test:
                log.debug( msg )
            else:
                log.error( msg )

            Type = -1
            Len = 0
            Data = 0

            return Type, Len, Data
            
        Type = d[0]
        Len = d[1]

        Data = self.dev.read( Len )

        if not len(Data) == Len:

            msg = 'Could not read {} byte frame content, ({} read), Timeout: {}'.format( Len, len(d), self.dev.timeout )
            if Test:
                log.debug( msg )
            else:
                log.error( msg )

            Type = -1
            Len = 0
            Data = 0
        

        log.debug( 'Success ReadFrame, Type = {}, Len = {}, Data::  '.format( Type, Len) + " ".join(hex(b) for b in Data ))

        return Type, Len, Data


    ####
    def WriteFrame( self, Type, Len, Data ):

        D = bytearray( [ Type, Len ] )
        
        if Len > 0:
            D.extend( Data )

        log.debug( "writing: " + "  :: " + " ".join(hex(b) for b in D ))

        try:
            nw = self.dev.write( D )

        except:
            msg = 'Could not write frame to serial device'
            log.error( msg )
            sys.exit( msg )

    ####
    def GetDeviceStatus( self, ACKwaittime = 0.01 ):

        out = dict()
        retval = True

        # ask for status frame

        Type = 0x01
        Len = 0x00
        Data = bytes(1) # dummy

        self.WriteFrame( Type, Len, Data )

        # read result

        typ, length, data = self.ReadFrame( TimeOut = ACKwaittime, Test = False )

        if( length != 4 or not typ == Type ):
            msg = 'Incorrect Device Status Ack data received.  Type = {}, Len = {}'.format( typ, length )
            retval = False
            log.error( msg )

        if retval:

            out[ 'Time' ] = struct.unpack( '<L', data )[0]

            # read ack frame

            typ, length, data = self.ReadFrame( TimeOut = ACKwaittime, Test = True )


            if typ == -1: # we did not get frame within timeout
                log.error( 'Did not get Ack frame within timout' )
                retval = False


        return retval, out


            
    ####
    def ReceiveFrame( self, TimeOut = None, Test = False ):

        ## wait for datagram on serial line (sent via radio or from ComNode in case of e.g. AirStat statistics)
        ## return datagram to caller

        log.debug( "ComNode: Enter ReceiveFrame()" )


        # wait for next frame
        typ, length, data = self.ReadFrame( TimeOut = TimeOut, Test = Test )

        return typ, length, data




    ####
    def Reset( self, ACKwaittime = 1 ):

        ## make & send LINEFRAMETYPE_RESET frame

        # frame format
        # uint8_t Type = AIRFRAMETYPE_RESET 0x09
        # uint8_t Len = 0

        retval = True

        Type = 0x04

        #Data = bytearray( struct.pack( '<L', PulseLen ) )
        Data = bytearray([])
        Len = len( Data ) #  not counting type/len bytes

        self.WriteFrame( Type, Len, Data )

        log.debug( 'Sent Reset frame' )


        #time.sleep(1)


        # read ack frame
        typ, length, data = self.ReadFrame( TimeOut = ACKwaittime, Test = True )

        log.debug( 'Reset Frame Ack, Type: {}, Length: {}, Data: {}'.format( typ, length, data) )

        if typ == -1:
            log.info( 'Reset did not get ack from radio peer' )

        else:

            AckFrameType = data[0]
            AckNack = data[1]


            if( AckNack == 0 ):
                msg = 'Could not issue Reset'
                log.error( msg )
                retval = False

            else:
                log.info( 'Reset sucess' )



        return retval

    ####
    def PulsePIN( self, PulseLen = 10, ACKwaittime = 1, Test = False ):

        ## make & send LINEFRAMETYPE_PULSEPIN frame

        # frame format
        # uint8_t Type = LINEFRAMETYPE_PULSEPIN = 0x07
        # uint8_t Len = 1*sizeof(uint32_t);


        retval = True

        Type = 0x03

        Data = bytearray( struct.pack( '<L', PulseLen ) )
        Len = len( Data ) #  not counting type/len bytes

        self.WriteFrame( Type, Len, Data )

        log.debug( 'Sent PulsePin frame' )


        # read ack frame

        typ, length, data = self.ReadFrame( TimeOut = ACKwaittime, Test = False )

        AckFrameType = data[0]
        AckNack = data[1]

        log.debug( 'PulsePin Frame Ack, Type: {}, Length: {}, Data: {}'.format( typ, length, data) )


        if( AckNack == 0 ):
            msg = 'Could not issue PulsePin'
            log.error( msg )
            retval = False

        else:
            log.info( 'PulsePin Cmd completed sucessfully' )

        return retval



    ####
    def GetTachoReport( self, ACKwaittime = 1, Test = False ):

        retval = True

        ## make & send LINEFRAMETYPE_PULSEPIN frame


        # frame format
        # uint8_t Type = LINEFRAMETYPE_TACHO_REPORT 0x05
        # uint8_t Len = 0 // no pars

        FrameType = 0x05 # LINEFRAMETYPE_TACHO_REPORT 

        Data = bytearray( [] )
        Len = len( Data ) #  not counting type/len bytes

        self.WriteFrame( FrameType, Len, Data )

        log.debug( 'Sent TachoReportFcn frame' )


        # read result frame

        typ, length, data = self.ReadFrame( TimeOut = ACKwaittime, Test = False )


        if typ == FrameType and length == 2*4:
            
            Time, Tacho = struct.unpack( '<LL', data )

        else:

            log.error( 'Unexpected frame type={} or length={}'.format( typ, length ))
            retval = False


        if retval:

            # read ack frame

            typ, length, data = self.ReadFrame( TimeOut = ACKwaittime, Test = False )

            log.debug( 'TachoReportFcn Frame Ack, Type: {}, Length: {}, Data: {}'.format( typ, length, data) )

            if typ == 0x02 and length == 2:

                AckFrameType, AckNack = struct.unpack( '<BB', data )


                if  not (AckFrameType == FrameType and AckNack == 1):
                    msg = 'Did not receive ACK for TachoReportFcn'
                    log.error( msg )
                    retval = False

                else:
                    log.debug( 'TachoReportFcn ack received successfully' )



        if not retval:

            Time = np.nan
            Tacho = np.nan

        return Time, Tacho
        
########################################################################








# Opdateringsfunktion
def update():
    global i
    global last_seconds
    global last_i

    line = ser.readline().decode('utf-8', errors='ignore').strip()
    match = pattern.search(line)
    if match:

        seconds = int(time.time())



        i += 1

        if(last_seconds < seconds):
            print(f"package/second: {i-last_i}")
            last_i = i
            last_seconds = seconds

        Time, Tacho = s.GetTachoReport()
        distance = Tacho/6139

        snr, rssi, freq = match.groups()
        snr = float(snr)
        rssi = float(rssi)

        if freq == "2450" or freq == "868":
            i_2_4.append(distance)
            snr_list_2_4.append(snr)
            rssi_list_2_4.append(rssi)
            rssi_curve_24.setData(i_2_4, rssi_list_2_4)
            snr_curve_24.setData(i_2_4, snr_list_2_4)
        else:
            i_433.append(distance)
            snr_list_433.append(snr)
            rssi_list_433.append(rssi)
            rssi_curve_433.setData(i_433, rssi_list_433)
            snr_curve_433.setData(i_433, snr_list_433)
        # Skriv data til CSV
        with open(csv_filename, mode="a", newline='') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow([seconds, distance, freq, snr, rssi])












if __name__ == "__main__":
    
    # Lav et timestamp til filnavnet ved opstart
    start_time_str = datetime.now().strftime("%Y%m%d_%H%M%S")
    csv_filename = f"målinger_{start_time_str}.csv"
    # Skriv header når filen oprettes
    with open(csv_filename, mode="w", newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["Time", "Distance", "Frequency", "SNR", "RSSI"])


    # 0.5 4301
    #28.5 1715136193
    #6193 counts pr meter

    app = QtWidgets.QApplication([])
    win = pg.GraphicsLayoutWidget(title="Live RSSI/SNR Plot")
    win.resize(1000, 600)
    plot_rssi = win.addPlot(title="RSSI (solid)")
    plot_rssi.setLabel('left', 'RSSI')
    plot_rssi.setLabel('bottom', 'Packet Index')
    plot_snr = win.addPlot(title="SNR (dotted)")
    plot_snr.setLabel('left', 'SNR')
    plot_snr.setLabel('bottom', 'Packet Index')

    # Curves to plot
    rssi_curve_24 = plot_rssi.plot(pen=pg.mkPen('b', width=2), name="RSSI 2.4GHz")
    rssi_curve_433 = plot_rssi.plot(pen=pg.mkPen('r', width=2), name="RSSI 433MHz")
    snr_curve_24 = plot_snr.plot(pen=pg.mkPen('b', style=QtCore.Qt.DotLine), name="SNR 2.4GHz")
    snr_curve_433 = plot_snr.plot(pen=pg.mkPen('r', style=QtCore.Qt.DotLine), name="SNR 433MHz")



    snr_list_2_4 = []
    rssi_list_2_4 = []

    snr_list_433 = []
    rssi_list_433 = []

    i_433 = []
    i_2_4 = []

    # Serial setup
    ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.1) ##Vores 2 serieller
    s = IOnode( SerDevStr='/dev/ttyUSB1' )

    logging.basicConfig(filename='./run.log', encoding='utf-8', \
                        format='%(asctime)s,%(msecs)d %(levelname)-8s [%(filename)s:%(lineno)d] %(message)s',
                        datefmt='%Y-%m-%d:%H:%M:%S', level=logging.DEBUG)


    
    
    



    pattern = re.compile(r"SNR:\s*([-\d.]+)\s+RSSI:\s*([-\d.]+)\s+FREQ:\s*(\d+)")
    i = 0
    last_seconds = 0
    last_i = 0

    timer = QtCore.QTimer()
    timer.timeout.connect(update)
    timer.start(10)  # opdatering hvert 10 ms

    # Vis vindue og start eventloop
    win.show()
    sys.exit(app.exec_())
