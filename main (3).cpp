#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Plato {
    string nombre;
    int precio;
};

class Pedido {
private:
    Plato* platos;
    size_t cant_platos;
    bool servir;
    int id_llevar;
    int mesa;

public:
    Pedido();
    ~Pedido();
    void agregar_plato(const Plato& plato);
    int precio_total() const;
    void reset_servir(bool tipo);
    void reset_id(int id);
    void reset_mesa(int numero_mesa);
    int get_id() const;
    int get_mesa() const;
    bool es_para_servir() const;
    void mostrar_pedido() const;
    bool esta_vacio() const;
};

Pedido::Pedido() {
    platos = new Plato[25];
    cant_platos = 0;
    servir = false;
    id_llevar = -1;
    mesa = -1;
}

Pedido::~Pedido() {
    delete[] platos;
}

void Pedido::agregar_plato(const Plato& plato) {
    if (cant_platos < 25) {
        platos[cant_platos++] = plato;
    } else {
        cout << "Alcanzo maximo de platos." << endl;
    }
}

int Pedido::precio_total() const {
    int total = 0;
    for (size_t i = 0; i < cant_platos; ++i) {
        total += platos[i].precio;
    }
    return total;
}

void Pedido::reset_servir(bool tipo) {
    servir = tipo;
}

void Pedido::reset_id(int id) {
    id_llevar = id;
}


void Pedido::reset_mesa(int numero_mesa) {
    if (numero_mesa <= 0) {
        cout << "Error: el número de mesa debe ser positivo." << endl;
    } else {
        mesa = numero_mesa;
    }
}



int Pedido::get_id() const {
    return servir ? mesa : id_llevar;
}

int Pedido::get_mesa() const {
    return mesa;
}

bool Pedido::es_para_servir() const {
    return servir;
}

void Pedido::mostrar_pedido() const {
    for (size_t i = 0; i < cant_platos; ++i) {
        printf("- %s : $%d\n", platos[i].nombre.c_str(), platos[i].precio);
    }
    printf("Precio total : $%d\n", precio_total());
}

class Registro {
private:
    Pedido* pedidos;
    bool* ocupadas;
    size_t size;
    size_t elementos;
    int ganancias;
    void ajustar_arreglo();

public:
    Registro();
    ~Registro();
    void agregar_pedido(Pedido* pedido);
    Pedido* get_pedido(int id, bool tipo);
    Pedido* eliminar_pedido(int id, bool tipo);
    size_t tam() const;
    void ajustar_tam(size_t tam);
    bool esta_ocupada(size_t i) const;
    Pedido* obtener_pedido(size_t i) const;
    int tener_ganancias() const;
};

Registro::Registro() {
    size = 10;
    ganancias = 0;
    elementos = 0;
    pedidos = new Pedido[size];
    ocupadas = new bool[size];

    for (size_t i = 0; i < size; ++i) {
        ocupadas[i] = false;
    }
}

Registro::~Registro() {
    delete[] pedidos;
    delete[] ocupadas;
}

void Registro::ajustar_arreglo() {
    size_t nueva_size = size * 1.5;
    Pedido* nueva_tabla = new Pedido[nueva_size];
    bool* nuevas_ocupadas = new bool[nueva_size];

    for (size_t i = 0; i < nueva_size; ++i) {
        nuevas_ocupadas[i] = false;
    }

    for (size_t i = 0; i < size; ++i) {
        if (ocupadas[i]) {
            int nueva_pos = pedidos[i].get_id() % nueva_size;
            while (nuevas_ocupadas[nueva_pos]) {
                nueva_pos = (nueva_pos + 1) % nueva_size;
            }
            nueva_tabla[nueva_pos] = pedidos[i];
            nuevas_ocupadas[nueva_pos] = true;
        }
    }

    delete[] pedidos;
    delete[] ocupadas;

    pedidos = nueva_tabla;
    ocupadas = nuevas_ocupadas;
    size = nueva_size;
}

void Registro::agregar_pedido(Pedido* pedido) {
    if (elementos * 1.0 / size > 0.8) {
        ajustar_arreglo();  // Ajustar el tamaño del arreglo si es necesario
    }

    int id = pedido->get_id();
    int pos = id % size;  // Cálculo del índice usando el hash

    while (ocupadas[pos]) {
        pos = (pos + 1) % size;  // Resolución de colisión lineal
    }

    // Verificación de la posición válida
    if (pos < size) {
        pedidos[pos] = *pedido;
        ocupadas[pos] = true;
        elementos++;
    } else {
        cout << "Error: posición inválida para agregar el pedido." << endl;
    }
}

Pedido* Registro::get_pedido(int id, bool tipo) {
    int pos = id % size;  // Cálculo del índice con el hash

    while (ocupadas[pos]) {
        if (pedidos[pos].get_id() == id && pedidos[pos].es_para_servir() == tipo) {
            return &pedidos[pos];  // Si el pedido es encontrado, lo devolvemos
        }
        pos = (pos + 1) % size;  // Resolución de colisión lineal
    }

    return nullptr;  // Si no lo encontramos, devolvemos nullptr
}

Pedido* Registro::eliminar_pedido(int id, bool tipo) {
    int pos = id % size;

    while (ocupadas[pos]) {
        if (pedidos[pos].get_id() == id && pedidos[pos].es_para_servir() == tipo) {
            Pedido* eliminado = new Pedido(pedidos[pos]);
            ocupadas[pos] = false;
            elementos--;
            ganancias += eliminado->precio_total();
            return eliminado;
        }
        pos = (pos + 1) % size;
    }

    return nullptr;
}

size_t Registro::tam() const {
    return size;
}

int Registro::tener_ganancias() const {
    return ganancias;
}

void Registro::ajustar_tam(size_t tam) {
    if (pedidos) delete[] pedidos;
    if (ocupadas) delete[] ocupadas;

    size = tam;
    pedidos = new Pedido[size];
    ocupadas = new bool[size];
    for (size_t i = 0; i < size; ++i) {
        ocupadas[i] = false;
    }
    elementos = 0;
}

bool Registro::esta_ocupada(size_t i) const {
    if (i < size) {
        return ocupadas[i];
    }
    return false;
}

Pedido* Registro::obtener_pedido(size_t i) const {
    if (i < size) {
        return &pedidos[i];
    }
    return nullptr;
}

// Variables globales
Plato* menu = nullptr;
int num_platos = 0;
Registro registro;
Pedido* pedido_actual = nullptr;

void leer_menu(const string& nombre_archivo) {
    ifstream archivo;

    archivo.open(nombre_archivo);
    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo. \n";
        exit(1);
    }

    archivo >> num_platos;
    archivo.ignore();

    menu = new Plato[num_platos];

    for (int i = 0; i < num_platos; i++) {
        string linea;
        getline(archivo, linea);

        size_t pos = linea.find('-');
        string nombre = linea.substr(0, pos);
        int precio = stoi(linea.substr(pos + 1));

        menu[i].nombre = nombre;
        menu[i].precio = precio;
    }

    archivo.close();
    cout << "Menu cargado exitosamente." << endl;
}

void ejecutar_comando(const string& comando) {
    size_t pos1 = comando.find(' ');
    string accion = comando.substr(0, pos1);

    if (accion == "registrar") {
        size_t pos2 = comando.find(' ', pos1 + 1);
        string tipo = comando.substr(pos1 + 1, pos2 - pos1 - 1);

        if (tipo == "mesa") {
            string id_str = comando.substr(pos2 + 1);
            int id = stoi(id_str);

            if (registro.get_pedido(id, true)) {
                cout << "La mesa ya tiene un pedido asignado." << endl;
            } else {
                pedido_actual = new Pedido();
                pedido_actual->reset_servir(true);
                pedido_actual->reset_mesa(id);
                cout << "mesa " << id << " Registrado" << endl;
            }
        } else if (tipo == "llevar") {
            static int id_llevar = 1;
            pedido_actual = new Pedido();
            pedido_actual->reset_servir(false);
            pedido_actual->reset_id(id_llevar++);
            cout << "llevar " << id_llevar - 1 << " Registrado" << endl;
        }
    } else if (accion == "agregar") {
        string nombre_plato = comando.substr(pos1 + 1);

        bool plato_encontrado = false;
        for (int i = 0; i < num_platos; i++) {
            if (menu[i].nombre == nombre_plato) {
                if (pedido_actual) {
                    pedido_actual->agregar_plato(menu[i]);
                    cout << "Plato '" << nombre_plato << "' agregado al pedido." << endl;
                } else {
                    cout << "No hay pedido en curso." << endl;
                }
                plato_encontrado = true;
                break;
            }
        }

        if (!plato_encontrado) {
            cout << "Plato no encontrado en el menu." << endl;
        }
    } else if (accion == "info") {
        size_t pos2 = comando.find(' ', pos1 + 1);
        string tipo = comando.substr(pos1 + 1, pos2 - pos1 - 1);
        string id_str = comando.substr(pos2 + 1);
        int id = stoi(id_str);

        bool flag = (tipo == "mesa");
       Pedido* pedido = registro.get_pedido(id, flag);

        if (pedido != nullptr) {
            // Si el pedido existe, mostramos los detalles
            pedido->mostrar_pedido();
        } else {
            // Si no se encuentra el pedido, mostramos un mensaje de error
            cout << "Pedido no encontrado para " << tipo << " " << id << "." << endl;
        }
    } else if (accion == "pagar") {
        size_t pos2 = comando.find(' ', pos1 + 1);
        string tipo = comando.substr(pos1 + 1, pos2 - pos1 - 1);
        string id_str = comando.substr(pos2 + 1);
        int id = stoi(id_str);

        Pedido* eliminado = registro.eliminar_pedido(id, tipo == "mesa");
        if (eliminado) {
            int total = eliminado->precio_total();
            int propina = total * 0.1;
            printf("Total: $%d, Propina: $%d, Total con propina: $%d\n", total, propina, total + propina);
            delete eliminado;
        } else {
            cout << "Pedido no encontrado." << endl;
        }
    } else if (accion == "pedir") {
        if (pedido_actual) {
            registro.agregar_pedido(pedido_actual);
            cout << "Pedido registrado: " << (pedido_actual->es_para_servir() ? "mesa " + to_string(pedido_actual->get_mesa()) : "llevar #" + to_string(pedido_actual->get_id())) << endl;
            delete pedido_actual;
            pedido_actual = nullptr;
        } else {
            cout << "No hay pedido en curso." << endl;
        }
    } else if (accion == "cerrar") {
        cout << "Pedidos pendientes:" << endl;
        for (size_t i = 0; i < registro.tam(); i++) {
            if (registro.esta_ocupada(i)) {
                Pedido* pedido = registro.obtener_pedido(i);
                if (pedido->es_para_servir()) {
                    cout << "mesa " << pedido->get_mesa() << endl;
                } else {
                    cout << "llevar #" << pedido->get_id() << endl;
                }
            }
        }
        printf("Ganancias del día: $%d\n", registro.tener_ganancias());
    } else {
        cout << "Comando no encontrado." << endl;
    }
}

int main() {
    leer_menu("Menu.txt");

    cout << "> ";
    int cantidad_mesas;

    // Validación de entrada para la cantidad de mesas
    while (!(cin >> cantidad_mesas) || cantidad_mesas <= 0) {
        cout << "Error: debe ingresar un número entero positivo para la cantidad de mesas." << endl;
        cout << "> ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore();

    // Ajustamos el tamaño de la tabla de pedidos
    registro.ajustar_tam(cantidad_mesas);

    string comando;
    while (true) {
        cout << "> ";
        getline(cin, comando);

        if (comando.empty()) {
            cout << "Error: no se puede ingresar un comando vacío. Intente nuevamente." << endl;
            continue;
        }

        ejecutar_comando(comando);

        if (comando == "cerrar") {
            cout << "Sistema cerrado. ¡Hasta luego!" << endl;
            break;
        }
    }

    delete[] menu;
    return 0;
}
