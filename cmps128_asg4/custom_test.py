import os
import sys
import unittest
import subprocess
import requests
import sys
import random
import time
import threading

# def printit():
# 	t = threading.Timer(0.5, printit)
# 	t.daemon = True
# 	t.start()
# 	print("Hello, World!")

# def addresses_to_string():
# 		addresses = [
# 			["10.0.0.3:8080", "10.0.0.4:8080"],
# 			["10.0.0.5:8080", "10.0.0.6:8080"]
# 		]
# 		retStr = ""
# 		for addressArray in addresses:
# 			tempIndex = 0
# 			for address in addressArray:
# 				if tempIndex == 0:
# 					retStr += address
# 				else:
# 					retStr += "," + address
# 				tempIndex += 1
# 			retStr += ";"
# 		return retStr[:-1]

# def string_to_addresses(addrString):
# 		partitionStrings = addrString.split(";")
# 		addresses = []
# 		for partitionString in partitionStrings:
# 			addresses.append(partitionString.split(","))
# 		return addresses

def main():
	key = "foo"
	res = requests.put('http://0.0.0.0:8082' +'/kvs',data = {'value':'bart', 'key':key})
	print (res.json())
	res = requests.get('http://localhost:8081/kvs?key=foo')
	print (res.json())
	res = requests.delete('http://0.0.0.0:8081' +'/kvs',data = {'key':key})
	print (res.json())


	# printit()

	# time.sleep(10)


	# addressToUpdate = "10.0.0.7:8080"
	# curView = "10.0.0.3:8080,10.0.0.4:8080,10.0.0.5:8080,10.0.0.6:8080,10.0.0.7:8080"

	# curView = curView.replace(addressToUpdate, "")
	# curView = curView.replace(",,", ",")
	# if curView[0] == ",":
	# 	curView = curView[1:]
	# if curView[len(curView) - 1] == ",":
	# 	curView = curView[:-1]

	# print(curView)

	# cancel_future_calls = call_repeatedly(0.5, print, "Hello, World")

	# print(string_to_addresses(addresses_to_string()))


	# addresses = []
	# curView = "10.0.0.3:8080,10.0.0.4:8080;;10.0.0.5:8080,10.0.0.6:8080;10.0.0.7:8080"
	# tempAddressesArray = curView.split(";")
	# for partitionString in tempAddressesArray:
	# 	print(partitionString)
	# 	addresses.append(partitionString.split(","))


	# keyValueStore = {}
	# numKeys = 0
	# numNodes = 0
	# myAddress = "10.0.0.20:8080"
	# curView = "10.0.0.20:8080,10.0.0.21:8080"
	# addresses = []
	# if (curView):
	# 	addresses = curView.split(",")
	# 	addresses = sorted(addresses)
	# 	for address in addresses:
	# 		numNodes += 1
	# 	file = open("a" + str(1), "w")
	# 	file.write(str(addresses) + "\n")
	# 	file.write(str(numNodes) + "\n")
	# 	file.close()

if __name__ == '__main__':
	main()