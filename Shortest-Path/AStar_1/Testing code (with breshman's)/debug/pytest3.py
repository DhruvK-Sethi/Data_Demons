import sqlite3
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import pyplot

row = 13734
col = 18622


class Test:
    def __init__(self):
        self.count = 0
        self.no = 0
        self.sum = 0
        self.messageError = None
        self.data = ""
        self.db = None
        self.stmt = None
        self.stml = None
        self.myArray = []
        self.numCoords = None
        self.coords = []
        self.lat_1 = 0
        self.long_1 = 0
        self.mapKey = 0

        self.db = sqlite3.connect("New_network.db")
        print("Opened database successfully")

        self.myArray = [[1 for _ in range(col)] for _ in range(row)]

        print("Array created successfully: ")

    def check(self):
        print("Checking...")
        sql_query1 = "SELECT double1, double2 FROM myTable;"
        self.stmt = self.db.cursor()
        self.stmt.execute(sql_query1)

        for row in self.stmt.fetchall():
            double1, double2 = row
            self.array(double1, double2)

        print("Process finished: ", self.sum)
        print(self.myArray[2603][3591]) 
        print(self.myArray[2568][3633]) 

        plt.imshow(self.myArray, cmap='hot')
        plt.colorbar()
        plt.show()


        # pyplot.figure(figsize=(10,10))
        # pyplot.imshow(self.myArray)
        # pyplot.show()

        print("Calling A-star...")
        
        print("Calling Failed...")


    def array(self, double1, double2):
        lat_1 = int((round(double1, 5)-76.66046)*100000)
        long_1 = int((round(double2, 5)-30.65364)*100000)    
        print("Latitude: ", lat_1)
        print("Longitude: ", long_1)
        # print("Value: ", self.myArray[lat_1][long_1])
        self.myArray[lat_1][long_1] = 0
        self.sum += 1

    def __del__(self):
        self.db.close()


if __name__ == "__main__":
    t = Test()
    t.check()
