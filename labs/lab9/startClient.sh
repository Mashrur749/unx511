echo "usage: startClient.sh <port number>"
./client1 $1 &
sleep 1
./client2 $1 &
sleep 1
./client3 $1 &
