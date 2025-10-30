#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <string>
#include <functional> // Para std::ref e std::cref
#include <vector>     // Para std::vector

// --- INCLUDES DOS COMPONENTES DO PROJETO ---
#include "BufferCircular.h" // Precisa da definição para criar o objeto
#include "dados.h"          // Precisa da definição dos structs
#include "utils.h"          // Precisa da função gerar_placa_mercosul()
#include "tarefas.h"        // Precisa das declarações das funções das tarefas

// --- Variáveis Globais de Estado e Sincronização (DEFINIÇÃO) ---
// Estas são as definições reais das variáveis declaradas como 'extern' em tarefas_caminhao.cpp
BufferCircular buffer_principal(200); // Define e constrói o buffer global com tamanho 200
std::string g_placa_caminhao;       // A placa será definida no início do main
bool g_modo_automatico = true;      // Começa em modo automático
bool g_falha_ativa = false;         // Começa sem falhas
bool g_alerta_proximidade = false;  // Começa sem alertas de proximidade
std::mutex mtx_estado;              // Define e constrói o mutex global para o estado

// --- Função Principal ---
int main() {
    // 1. Inicializa a identificação do caminhão
    g_placa_caminhao = gerar_placa_mercosul();
    std::cout << "======================================================" << std::endl;
    std::cout << " Iniciando Sistema Embarcado do Caminhão Autônomo" << std::endl;
    std::cout << " Placa: " << g_placa_caminhao << std::endl;
    std::cout << "======================================================" << std::endl;

    // 2. Cria um vetor para guardar os objetos std::thread
    std::vector<std::thread> tarefas;

    // 3. Cria e inicia cada thread, passando os argumentos necessários
    //    - std::ref() é usado para passar o buffer por referência (pode ser modificado)
    //    - std::cref() é usado para passar a placa por referência constante (não será modificada)
    std::cout << "Iniciando threads das tarefas..." << std::endl;
    try {
        tarefas.emplace_back(tarefa_tratamento_sensores, std::ref(buffer_principal), std::cref(g_placa_caminhao));
        tarefas.emplace_back(tarefa_logica_comando, std::ref(buffer_principal));
        tarefas.emplace_back(tarefa_controle_navegacao, std::ref(buffer_principal));
        tarefas.emplace_back(tarefa_monitor_falhas); // Não precisa de argumentos
        tarefas.emplace_back(tarefa_coletor_dados, std::ref(buffer_principal));
        tarefas.emplace_back(tarefa_interface_local); // Não precisa de argumentos
        // Adicione aqui a thread tarefa_planejamento_rota se for implementá-la
        // tarefas.emplace_back(tarefa_planejamento_rota);

    } catch (const std::system_error& e) {
        std::cerr << "Erro ao criar thread: " << e.what() << " (" << e.code() << ")" << std::endl;
        return 1; // Termina se não conseguir criar as threads
    }

    std::cout << "Todas as " << tarefas.size() << " tarefas foram iniciadas com sucesso." << std::endl;
    std::cout << "Pressione Ctrl+C para encerrar." << std::endl;
    std::cout << "======================================================" << std::endl;


    // 4. Espera que todas as threads terminem.
    //    Como as tarefas são loops infinitos, o programa principal ficará aqui indefinidamente.
    //    Isso é o comportamento esperado para um sistema embarcado contínuo.
    for (auto& t : tarefas) {
        if (t.joinable()) { // Verifica se a thread ainda pode ser juntada
            t.join();       // Bloqueia a thread principal até que a thread 't' termine
        }
    }

    // O código abaixo normalmente não seria alcançado em um loop infinito,
    // mas é uma boa prática incluí-lo.
    std::cout << "Encerrando o sistema embarcado." << std::endl;
    return 0;
}