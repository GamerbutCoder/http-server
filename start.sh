gcc -c -o ./lib/request ./lib/request.c
echo "Compiled ./lib/request.c"

gcc -c -o ./lib/response ./lib/response.c
echo "Compiled ./lib/response.c"

gcc -c -o ./lib/method ./lib/method.c
echo "Compiled ./lib/method.c"

gcc server.c -o server ./lib/request ./lib/response ./lib/method
echo "Compiled server.c"

echo "Starting server"

./server