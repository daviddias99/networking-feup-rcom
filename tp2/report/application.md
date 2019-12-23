### Aplicação
A aplicação aceita como argumento um endereço de um ficheiro presente num servidor FTP no seguinte formato:`ftp://[<user>:<password>@]<host>/<url-path>`.
E descarrega esse mesmo ficheiro (qualquer tipo de ficheiro pode ser descarregado). Todas as respostas do servidor são impressas na consola, assim como uma barra de progresso para que o utilizador saiba em qualquer momento o estado do programa.
Durante o desenvolvimento da aplicação foram consultados o RFC959, referente ao protocolo de transferência de dados e o RFC1738, referente ao tratamento de informação proveniente de URLs.

#### Arquitetura
Inicialmente é feito o processamento do url fornecido, de forma a assegurar que se encontra com o formato correto, a função responsável por esta tarefa é denominada de `parse_arguments`. E a informação extraída é colocada na seguinte estrutura:
````c
struct ftp_st {
    // ftp://[<user>:<password>@]<host>/<url-path>
    char* user;
    char* password;
    char* host;
    char* url_path;
};
````
No caso de não serem especificados o nome de utilizador e a palavra-passe o programa irá correr em modo anónimo(a variável user e password assumem o valor anonymous).
O endereço de ip do servidor é obtido através do hostname com recurso à função `getip` fornecida pelos professores. A porta utilizada é sempre a 21.
De seguida é aberto o socket através do qual é feita a comunicação entre o cliente e o servidor e são enviados os comandos `USER user` e `PASS pass` para efetuar o login. Se as credenciais forem válidas é enviado o comando `PASV` que efetua a entrada em modo passivo, ao processar a informação recebida do servidor obtemos a porta necessária para a abertura do segundo socket que tem como finalidade a transferência de dados.
É enviado o comando `RETR filepath` que faz o pedido do ficheiro especificado ao servidor e finalmente é efetuada a transferência do ficheiro com a chamada da função `download_file`.
No final da execução é enviado o comando `QUIT` para o servidor e após a receber uma resposta são fechadas todas as conexões.

Uma das principais funções é a `send_command` que envia um comando para o servidor e interpreta a resposta através da função `read_response`. Através do primeiro digito recebido é possível saber se a resposta é positiva (1, 2, 3) ou negativa (4 e 5). No caso de ser 1 o `read_response` é chamado novamente para ler a resposta seguinte, caso seja 2 ou 3 retorna, caso seja 4 tenta ler a resposta novamente e caso seja 5 termina a execução do programa.

#### Relatório de um download bem sucedido
imagem no grupo
