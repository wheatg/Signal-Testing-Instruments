import time
import serial

# configure the serial connections
ser = serial.Serial('COM11')
print(ser.name) 
#set settings for timeout
ser.write_timeout = 3
ser.timeout = 3
#set the initial state to nothing
STATE = ''
#list for storing last measurements
last_measurements = []


#main loop
while True:
    '''
    Exit state, stop script
    '''
    if STATE == 'E':
        ser.write('X'.encode('utf-8'))
        break
    elif STATE == 'X': #reset state resets master slave select and returns
        ser.write('X'.encode('utf-8'))
        print("Slave selection was reset")
        STATE == ''
    elif STATE == 'R': #read state for reading measurements
        #Select slave A-F
        print("Please Select a Slave(A-F):")
       #wait for an input
        slave = input(">>")
        #make sure the slave is valid
        if slave in ['A','B','C','D','E','F']:
            #writes selected slave 
            ser.write(slave.encode('utf-8'))
            print("you are now reading slave " + slave)
            #Sends Read Command 'R"
            ser.write('R'.encode('utf-8'))
            #read serial port until ‘?’ recieved
            readStr = ser.read_until('?').decode('utf-8')
            #split up the lines by ‘\n’
            StrList = readStr.splitlines()
            #prints list of last measurements
            print(StrList[:-2])
            #deselects slave
            ser.write('X'.encode('utf-8'))
        else:
            #Returns to menu if slave does not exist
            print("not a recognized slave, please try again")
        print("Read Complete\n")
        STATE = '' #returns to menu
    elif STATE == 'C':
        #Select slave A-F
        #loop while incorrect inputs
        while True:
            print("you are now commanding slave " + slave)
            print("Please Choose a command:")
            print("    F: Frequency Measurement")
            print("    P: Period Measurement")
            print("    S: Spectrum")
            print("    I: Interval")
            print("    E: Events")
            print("    X: Back to Main")
            choice = input(">>")
            #check for command characters
            if choice in ['F','P','S','I', 'E']:
                #write the command
                ser.write(choice.encode('utf-8'))
                break
            elif choice == 'X':
                ser.write('X'.encode('utf-8'))
                STATE = '' #returns to menu
                break
        #give time to transmit
        time.sleep(1)
        print("Command Complete\n")
    elif STATE == 'S': #select state, occurs before command once
        while True:
            #Select slave A-F
            print("Please Select a Slave(A-F):")
            slave = input(">>")
            if slave in ['A','B','C','D','E','F']:
                ser.write(slave.encode('utf-8'))
                STATE = 'C'
                break
            else:
                print("not a recognized slave, please try again")
    else:
        #main menu
        print("Please Choose What to do:")
        print("'R' = READ")
        print("'C' = COMMAND")
        print("'X' = RESET")
        print("'E' = EXIT")
        command = input(">>")
        if command == 'C':
            command = 'S'
        STATE = command
#close the serial port
ser.close() 