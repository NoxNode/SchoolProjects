import os
import sys
import requests
import time
import json
import threading
from flask import Flask
from flask import request
from flask import jsonify
from flask import session
app = Flask(__name__)

LONG_TIMEOUT=1
QUICK_TIMEOUT=0.2
PING_INTERVAL=0.5

keyValueStore = {}
numKeys = 0
numNodes = 0
numPartitions = 0
partition_id = 0
addresses = []
unavailable_nodes = []
most_recent_timestamp = 0
vector_clock = []

myAddress = os.getenv("ip_port")
curView = os.getenv("VIEW")
replicasPerPartition = int(os.getenv("K"))
if (curView): # skip this if we are a node that is about to be added and doesn't have the env variables
	# update view string to have ;'s instead of ,'s where partitions end / begin
	numCommasPassed = 0
	for i in range(0, len(curView)):
		if curView[i] == ",":
			numCommasPassed += 1
			if numCommasPassed != 0 and numCommasPassed % replicasPerPartition == 0:
				curView = curView[:i] + ';' + curView[i + 1:]

	# get the addresses, numPartition, and partition_id from this string
	partitionStrings = curView.split(";")
	curPartition = 0
	for partitionString in partitionStrings:
		addresses.append(partitionString.split(","))
		if myAddress in partitionString:
			partition_id = curPartition
		curPartition += 1
	numPartitions = len(addresses)
	curPartition = 0
	for addressArray in addresses:
		vector_clock.append([])
		for address in addressArray:
			vector_clock[curPartition].append(0)
		curPartition += 1

# no need to update for hw4
@app.route("/kvs/get_number_of_keys", methods = ["GET"])
def get_number_of_keys():
		global numKeys
		return jsonify(
			count = numKeys
		), 200

# new func for hw4
@app.route("/kvs/kvs_ping", methods = ["GET"])
def kvs_ping():
		global vector_clock
		their_vector_clock_string = request.values.get("vector_clock")
		their_vector_clock = string_to_clock(their_vector_clock_string)
		if compare_clocks(their_vector_clock, vector_clock) > 0:
			vector_clock = their_vector_clock
		return jsonify(
			msg = "success"
		), 200

# new func for hw4
def ping():
	global most_recent_timestamp
	global addresses
	global myAddress
	global partition_id
	global unavailable_nodes
	global keyValueStore
	global numKeys
	global vector_clock
	t = threading.Timer(PING_INTERVAL, ping)
	t.daemon = True
	t.start()
	if partition_id != -1:
		for address in addresses[partition_id]:
			if address != myAddress:
				# ping this smol boi
				try:
					res = requests.get("http://" + address + "/kvs/kvs_ping?vector_clock=" + clock_to_string(), timeout=LONG_TIMEOUT)
					# our boi did respond, so if he's in the unavailable_nodes array, take him out and update him
					if address in unavailable_nodes:
						unavailable_nodes.remove(address)
						res = requests.put("http://" + address + "/kvs/internal_update_replica", data = {"kvs":json.dumps(keyValueStore), "numKeys":numKeys,"most_recent_timestamp":most_recent_timestamp,"vector_clock":clock_to_string()}, timeout = LONG_TIMEOUT)
						# now our boi has been updated
				except:
					# our boi didn't respond in time, so add him to the unavailable_nodes array if he's not already there
					if address not in unavailable_nodes:
						unavailable_nodes.append(address)

def main():
	# wait 2 seconds and then start pinging nodes to make sure they're available
	t = threading.Timer(2, ping)
	t.daemon = True
	t.start()
	app.run(host="0.0.0.0", port=8080, debug=False)

# new func for hw4
def compare_clocks(clock1, clock2):
	if len(clock1) < len(clock2):
		return -1
	elif len(clock1) > len(clock2):
		return 1
	for i in range(0, len(clock1)):
		for j in range(0, len(clock1[i])):
			if clock1[i][j] < clock2[i][j]:
				return -1
			elif clock1[i][j] > clock2[i][j]:
				return 1
	return 0

# new func for hw4
def inc_my_clock():
	global vector_clock
	global addresses
	global myAddress
	global partition_id
	tempIndex = 0
	if partition_id >= 0:
		for address in addresses[partition_id]:
			if address == myAddress:
				vector_clock[partition_id][tempIndex] += 1
				break
			tempIndex += 1

# new func for hw4
def clock_to_string():
		global vector_clock
		retStr = ""
		for clockArray in vector_clock:
			tempIndex = 0
			for clock in clockArray:
				if tempIndex == 0:
					retStr += str(clock)
				else:
					retStr += "," + str(clock)
				tempIndex += 1
			retStr += ";"
		return retStr[:-1]

# new func for hw4
def string_to_clock(clockString):
		clockStrings = clockString.split(";")
		temp_vector_clock = []
		tempIndex = 0
		for clocks in clockStrings:
			temp_clocks_string = clocks.split(",")
			temp_vector_clock.append([])
			for clock in temp_clocks_string:
				temp_vector_clock[tempIndex].append(int(clock))
			tempIndex += 1
		return temp_vector_clock

# new func for hw4
def addresses_to_string():
		global addresses
		retStr = ""
		for addressArray in addresses:
			tempIndex = 0
			for address in addressArray:
				if tempIndex == 0:
					retStr += address
				else:
					retStr += "," + address
				tempIndex += 1
			retStr += ";"
		return retStr[:-1]

# new func for hw4
def string_to_addresses(addrString):
		global addresses
		partitionStrings = addrString.split(";")
		addresses = []
		for partitionString in partitionStrings:
			addresses.append(partitionString.split(","))

# new func for hw4
@app.route("/kvs/get_partition_id", methods = ["GET"])
def get_partition_id():
		global partition_id
		return jsonify(
			msg = "success",
			partition_id = partition_id
		), 200

# new func for hw4
@app.route("/kvs/get_all_partition_ids", methods = ["GET"])
def get_all_partition_ids():
		global addresses
		partition_id_list = []
		tempIndex = 0
		for addressArray in addresses:
			partition_id_list.append(tempIndex)
			tempIndex += 1
		return jsonify(
			msg = "success",
			partition_id_list = partition_id_list
		), 200

# new func for hw4
@app.route("/kvs/get_partition_members", methods = ["GET"])
def get_partition_members():
		global addresses
		temp_partition_id = int(request.values.get("partition_id"))
		return jsonify(
			msg = "success",
			partition_members = addresses[temp_partition_id]
		), 200

# updated for hw4
@app.route("/kvs/internal_del_key", methods = ["PUT"])
def internal_del_key():
		global numKeys
		global keyValueStore
		global addresses
		global myAddress
		key = request.values.get("key")
		if key not in keyValueStore:
			return jsonify(
				msg = "error",
				error = "key does not exist"
			), 404
		del keyValueStore[key]
		return jsonify(
			msg = 'success'
		), 200

# new func for hw4
@app.route("/kvs/internal_give_keys", methods = ["PUT"])
def internal_give_keys():
		global numKeys
		global keyValueStore
		global addresses
		global myAddress
		kvs = json.loads(request.values.get("kvs"))
		for key in kvs:
			if key not in keyValueStore:
				numKeys += 1
			else:
				print ("this shouldnt print", file=sys.stderr)
			keyValueStore[key] = kvs[key]
		return jsonify(
			msg = 'success'
		), 200

# updated for hw4
@app.route("/kvs/internal_get_key", methods = ["GET"])
def internal_get_key():
		global numKeys
		global keyValueStore
		global addresses
		global myAddress
		key = request.values.get("key")
		if key not in keyValueStore:
			return jsonify(
				msg = "error",
				error = "key does not exist"
			), 404
		return jsonify(
			msg = 'success',
			value = keyValueStore[key]
		), 200

# updated for hw4
@app.route("/kvs/internal_give_key", methods = ["PUT"])
def internal_give_key():
		global numKeys
		global keyValueStore
		global addresses
		global myAddress
		key = request.values.get("key")
		value = request.values.get("value")
		if key not in keyValueStore:
			numKeys += 1
		else:
			print ("this shouldnt print", file=sys.stderr)
		keyValueStore[key] = value
		return jsonify(
			msg = 'success'
		), 200

# updated for hw4
@app.route("/kvs/internal_give_your_keys", methods = ["PUT"])
def internal_give_your_keys():
	return give_my_keys(request.values.get("sender_partition"), request.values.get("sender"))

# updated for hw4
@app.route("/kvs/internal_view_update", methods = ["PUT"])
def internal_view_update():
		global numNodes
		global curView
		global addresses
		global myAddress
		global replicasPerPartition
		global partition_id
		global numPartitions
		global vector_clock
		replicasPerPartition = int(request.values.get('K'))
		string_to_addresses(request.values.get('addresses'))
		partition_id = -1
		curPartition = 0
		vector_clock = []
		numNodes = 0
		for addressArray in addresses:
			vector_clock.append([])
			for address in addressArray:
				vector_clock[curPartition].append(0)
				if address == myAddress:
					partition_id = curPartition
				numNodes += 1
			curPartition += 1
		# update numPartitions
		numPartitions = len(addresses)
		return jsonify(
			msg = 'success'
		), 200

# updated for hw4
@app.route("/kvs/internal_update_replica", methods = ["PUT"])
def internal_update_replica():
		global numNodes
		global curView
		global addresses
		global myAddress
		global replicasPerPartition
		global partition_id
		global numPartitions
		global keyValueStore
		global numKeys
		global most_recent_timestamp
		global vector_clock
		their_vector_clock_string = request.values.get("vector_clock")
		their_vector_clock = string_to_clock(their_vector_clock_string)
		their_most_recent_timestamp = float(request.values.get("most_recent_timestamp"))
		if compare_clocks(their_vector_clock, vector_clock) < 0:
			# theirs is less, so don't override our kvs
			return jsonify(
				msg = 'success'
			), 200
		elif compare_clocks(their_vector_clock, vector_clock) == 0:
			# their equal, so compare timestamps
			if most_recent_timestamp > their_most_recent_timestamp:
				return jsonify(
					msg = 'success'
				), 200
		kvs = json.loads(request.values.get("kvs"))
		keyValueStore = kvs
		numKeys = int(request.values.get("numKeys"))
		return jsonify(
			msg = 'success'
		), 200

# updated for hw4
def give_my_keys(sender_partition, sender):
		global numPartitions
		global numKeys
		global keyValueStore
		global myAddress
		global addresses
		global partition_id
		global most_recent_timestamp
		global vector_clock
		kvsArray = []
		for addressArray in addresses:
			kvsArray.append({})
		localKeyValueStore = keyValueStore.copy()
		for key in localKeyValueStore:
			keyHash = hash(key) % numPartitions
			if keyHash != partition_id:
				kvsArray[keyHash][key] = localKeyValueStore[key]
				del keyValueStore[key]
				numKeys -= 1
		curPartition = 0
		for addressArray in addresses:
			if curPartition == partition_id:
				for address in addressArray:
					if address != myAddress:
						try:
							res = requests.put('http://' + address + "/kvs/internal_update_replica", data = {"kvs":json.dumps(keyValueStore), "numKeys":numKeys,"most_recent_timestamp":most_recent_timestamp,"vector_clock":clock_to_string()})
						except:
							return jsonify(
								msg = "error",
								error = "failed to update replicas"
							), 404
			else:
				for address in addressArray:
					if address != sender:
						try:
							res = requests.put('http://' + address + "/kvs/internal_give_keys", data = {"kvs":json.dumps(kvsArray[curPartition])})
						except:
							return jsonify(
								msg = "error",
								error = "failed to give keys to other partitions"
							), 404
			curPartition += 1
		if int(sender_partition) == partition_id:
			return jsonify(
				msg = 'success'
			), 200
		return jsonify(
			kvsArray[int(sender_partition)]
		), 200

# updated for hw4
@app.route("/kvs/view_update", methods = ["PUT"])
def view_update():
		global numNodes
		global curView
		global addresses
		global myAddress
		global numKeys
		global keyValueStore
		global numPartitions
		global replicasPerPartition
		global partition_id
		updateType = request.values.get('type')
		addressToUpdate = request.values.get('ip_port')
		oldNumPartitions = numPartitions
		newNodePartitionId = 0
		addressNeedingToGiveKeys = ""
		if updateType == "remove":
			# first record info about the node we're about to remove and any other partitions with less than K replicas
			partitionWithLessThanKReplicas = -1
			indexToRemove = False
			partitionToRemoveFrom = -1
			curPartition = 0
			for addressArray in addresses:
				if len(addressArray) < replicasPerPartition:
					partitionWithLessThanKReplicas = curPartition
				tempIndex = 0
				for address in addressArray:
					if address == addressToUpdate:
						indexToRemove = tempIndex
						partitionToRemoveFrom = curPartition
						break
					tempIndex += 1
				curPartition += 1
			# then remove the address
			addresses[partitionToRemoveFrom].remove(addressToUpdate)
			# then handle edge cases
			if len(addresses[partitionToRemoveFrom]) == 0:
				# we've removed the last node from a partition
				numPartitions -= 1
				addressNeedingToGiveKeys = addressToUpdate
				del addresses[partitionToRemoveFrom]
			elif partitionWithLessThanKReplicas != -1 and partitionWithLessThanKReplicas != partitionToRemoveFrom:
				# we got 2 partitions with less than K replicas
				lesserPartitionId = 0
				greaterPartitionId = 0
				if partitionWithLessThanKReplicas < partitionToRemoveFrom:
					lesserPartitionId = partitionWithLessThanKReplicas
					greaterPartitionId = partitionToRemoveFrom
				else:
					lesserPartitionId = partitionToRemoveFrom
					greaterPartitionId = partitionWithLessThanKReplicas
				# move a node from the greater partition id to the lesser partition id
				addressToMove = addresses[greaterPartitionId][0]
				addresses[lesserPartitionId].append(addressToMove)
				addresses[greaterPartitionId].remove(addressToMove)
				# now that we removed a node from a partition, we need to handle the case
				# that we just removed the only node in that partition
				if len(addresses[partitionToRemoveFrom]) == 0:
					addressNeedingToGiveKeys = addressToUpdate
					numPartitions -= 1
					del addresses[partitionToRemoveFrom]
				if len(addresses[partitionWithLessThanKReplicas]) == 0:
					addressNeedingToGiveKeys = addressToMove
					numPartitions -= 1
					del addresses[partitionWithLessThanKReplicas]
		else: #type == "add"
			addedNewNodeToExistingPartition = False
			curPartition = 0
			# add to existing partition if available
			for addressArray in addresses:
				if len(addressArray) < replicasPerPartition:
					addedNewNodeToExistingPartition = True
					addresses[curPartition].append(addressToUpdate)
					newNodePartitionId = curPartition
					addressNeedingToGiveKeys = addressArray[0]
					break
				curPartition += 1
			# add to new partition if necessary
			if not addedNewNodeToExistingPartition:
				newNodePartitionId = numPartitions
				numPartitions += 1
				addresses.append([addressToUpdate])
		# now update every other node's view and update partition_id of this node
		addrString = addresses_to_string()
		curPartition = 0
		vector_clock = []
		for addressArray in addresses:
			vector_clock.append([])
			for address in addressArray:
				vector_clock[curPartition].append(0)
				if address != myAddress:
					try:
						res = requests.put('http://' + address + "/kvs/internal_view_update", data = {"addresses": addrString, "K": replicasPerPartition})
					except:
						return jsonify(
							msg = 'error',
							error = 'failed to update view'
						), 404
				else:
					partition_id = curPartition
			curPartition += 1
		# if we removed the last node in a partition, tell it to redistribute its keys
		if addressNeedingToGiveKeys != "" and addressNeedingToGiveKeys != myAddress:
			try:
				res = requests.put('http://' + addressNeedingToGiveKeys + "/kvs/internal_view_update", data = {"addresses": addrString, "K": replicasPerPartition})
				res = requests.put('http://' + addressNeedingToGiveKeys + "/kvs/internal_give_your_keys", data = {"sender_partition": partition_id, "sender": myAddress})
				kvs = res.json()
				for key in kvs:
					if key not in keyValueStore:
						numKeys += 1
					else:
						print ("this shouldnt print", file=sys.stderr)
					keyValueStore[key] = kvs[key]
			except:
				return jsonify(
					msg = 'error',
					error = 'failed to redistribute keys1'
				), 404
		elif addressNeedingToGiveKeys != "" and addressNeedingToGiveKeys == myAddress:
			give_my_keys(partition_id, myAddress)
		# redistribute the rest of the keys if the number of partitions changed
		if numPartitions != oldNumPartitions:
			curPartition = 0
			# first redistribute all of the other partitions' keys
			for addressArray in addresses:
				if curPartition != partition_id:
					try:
						res = requests.put('http://' + addressArray[0] + "/kvs/internal_give_your_keys", data = {"sender_partition": partition_id, "sender": myAddress})
						kvs = res.json()
						for key in kvs:
							if key not in keyValueStore:
								numKeys += 1
							else:
								print ("this shouldnt print", file=sys.stderr)
							keyValueStore[key] = kvs[key]
					except:
						return jsonify(
							msg = 'error',
							error = 'failed to redistribute keys2'
						), 404
				curPartition += 1
			# then redistribute all of this partition's keys
			give_my_keys(partition_id, myAddress)
		if updateType == "add":
			return jsonify(
				msg = "success",
				number_of_partitions = numPartitions,
				partition_id = newNodePartitionId
			), 200
		else:
			return jsonify(
				msg = "success",
				number_of_partitions = numPartitions
			), 200

# updated for hw4
@app.route("/kvs", methods = ["GET", "PUT", "DELETE"])
def kvs():
		global numPartitions
		global numKeys
		global keyValueStore
		global addresses
		global myAddress
		global partition_id
		global most_recent_timestamp
		global vector_clock
		their_vector_clock_string = request.values.get("causal_payload")
		if (their_vector_clock_string) and their_vector_clock_string != "":
			their_vector_clock = string_to_clock(their_vector_clock_string)
			if compare_clocks(their_vector_clock, vector_clock) > 0:
				# their vector clock is bigger, so make ours = theirs
				vector_clock = their_vector_clock
		key = request.values.get('key')
		if not (key):
			return jsonify(
				msg = 'error',
				error = 'request must include a key'
			), 404
		if len(key) > 250:
			return jsonify(
				msg = 'error',
				error = 'key longer than 250 characters'
			), 404
		keyInStoreAlready = False
		if key in keyValueStore.keys():
			keyInStoreAlready = True
		value = request.values.get('value')
		keyHash = hash(key) % numPartitions
		if request.method == "GET":
			if keyHash == partition_id:
				inc_my_clock()
				# we're the server that has the key, so get it
				if keyInStoreAlready:
					return jsonify(
						msg = 'success',
						value = keyValueStore[key],
						partition_id = partition_id,
						causal_payload = clock_to_string(),
						timestamp = time.time()
					), 200
				# check if other nodes in this partition have it
				for address in addresses[keyHash]:
					if address != myAddress:
						try:
							res = requests.get('http://' + address + "/kvs/internal_get_key?key=" + key, timeout=QUICK_TIMEOUT)
							if res.json()["value"]:
								numKeys += 1
								keyValueStore[key] = res.json()["value"]
								return jsonify(
									msg = 'success',
									value = keyValueStore[key],
									partition_id = partition_id,
									causal_payload = clock_to_string(),
									timestamp = time.time()
								), 200
						except:
							print ("failed to check if the key exists with other replica in GET", file=sys.stderr)
				return jsonify(
					msg = 'error',
					error = 'key does not exist'
				), 404
			else: # forward the request to the partition with the key
				try:
					res = requests.get('http://' + addresses[keyHash][0] + "/kvs?key=" + key, timeout=LONG_TIMEOUT)
					return jsonify(res.json()), res.status_code
				except:
					for i in range(1, len(addresses[keyHash])):
						try:
							res = requests.get('http://' + addresses[keyHash][i] + "/kvs?key=" + key, timeout=LONG_TIMEOUT)
							if res.json()["msg"] == "success":
								return jsonify(res.json()), 200
						except:
							print ("another one bites ze dustu", file=sys.stderr)
					return jsonify(
						msg = 'error',
						error = 'failed to relay GET'
					), 404
		if request.method == "PUT":
			if not (value):
				return jsonify(
					msg = 'error',
					error = 'PUT request with no value'
				), 404
			if keyHash == partition_id:
				inc_my_clock()
				most_recent_timestamp = time.time()
				# relay to other replicas in this partition
				for address in addresses[keyHash]:
					if address != myAddress:
						try:
							res = requests.put('http://' + address + "/kvs/internal_give_key", data = {'key':key, 'value':value}, timeout=QUICK_TIMEOUT)
						except:
							print ("failed to update replica with PUT", file=sys.stderr)
				# we're the server that has the key, so put it
				if keyInStoreAlready:
					keyValueStore[key] = value
					return jsonify(
						replaced = 1,
						msg = 'success',
						partition_id = partition_id,
						causal_payload = clock_to_string(),
						timestamp = time.time()
					), 200
				keyValueStore[key] = value
				numKeys += 1
				return jsonify(
					replaced = 0,
					msg = 'success',
					partition_id = partition_id,
					causal_payload = clock_to_string(),
					timestamp = time.time()
				), 201
			else: # forward the request to the partition with the key
				try:
					res = requests.put('http://' + addresses[keyHash][0] + "/kvs", data = {'key':key, 'value':value}, timeout=LONG_TIMEOUT)
					return jsonify(res.json()), res.status_code
				except:
					for i in range(1, len(addresses[keyHash])):
						try:
							res = requests.put('http://' + addresses[keyHash][i] + "/kvs", data = {'key':key, 'value':value}, timeout=LONG_TIMEOUT)
							if res.json()["msg"] == "success":
								return jsonify(res.json()), 200
						except:
							print ("another one bites ze dustu2", file=sys.stderr)
					return jsonify(
						msg = 'error',
						error = 'failed to relay PUT'
					), 404
		if request.method == "DELETE":
			if keyHash == partition_id:
				inc_my_clock()
				most_recent_timestamp = time.time()
				# relay to other replicas in this partition
				for address in addresses[keyHash]:
					if address != myAddress:
						try:
							res = requests.put('http://' + address + "/kvs/internal_del_key", data = {'key':key}, timeout=QUICK_TIMEOUT)
						except:
							print ("failed to update replica with DELETE", file=sys.stderr)
				# we're the server that has the key, so delete it
				if keyInStoreAlready:
					del keyValueStore[key]
					numKeys -= 1
					return jsonify(
						msg = 'success',
						partition_id = partition_id,
						causal_payload = clock_to_string(),
						timestamp = time.time()
					), 200
				return jsonify(
					msg = 'error',
					error = "key does not exist"
				), 404
			else: # forward the request to the partition with the key
				try:
					res = requests.delete('http://' + addresses[keyHash][0] + "/kvs", data = {'key':key}, timeout=LONG_TIMEOUT)
					return jsonify(res.json()), res.status_code
				except:
					for i in range(1, len(addresses[keyHash])):
						try:
							res = requests.delete('http://' + addresses[keyHash][i] + "/kvs", data = {'key':key}, timeout=LONG_TIMEOUT)
							if res.json()["msg"] == "success":
								return jsonify(res.json()), 200
						except:
							print ("another one bites ze dustu", file=sys.stderr)
					return jsonify(
						msg = 'error',
						error = 'failed to relay DELETE'
					), 404
    
if __name__ == "__main__":
    status = main()
    sys.exit(status)

