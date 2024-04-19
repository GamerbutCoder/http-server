rm ./lib/request
gcc -c -o ./lib/request ./lib/request.c
echo "Compiled ./lib/request.c"

rm ./lib/response
gcc -c -o ./lib/response ./lib/response.c
echo "Compiled ./lib/response.c"

rm ./lib/method
gcc -c -o ./lib/method ./lib/method.c
echo "Compiled ./lib/method.c"

rm ./server
gcc server.c -o server ./lib/request ./lib/response ./lib/method
echo "Compiled server.c"
