/*1.Implementar o cliente MQTT.
2. Conectar-se ao broker (Mosquitto).
3. Usar a biblioteca Paho MQTT C++.
4. Em um loop (controlado por uma std::thread):
5. Chamar simulador_logica.atualizarEstado() para calcular a nova posição.
6. Chamar simulador_logica.getDadosJson() para obter os dados.
7. Publicar a string JSON recebida no tópico MQTT correto.
8. Gerenciar reconexão em caso de falha.*/