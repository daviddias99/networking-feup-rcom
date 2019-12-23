# Experiência 6

## Quantas conexões TCP são abertas pela sua aplicação FTP? (How many TCP connections are opened by your ftp application?)
## Em qual das conexões é transportada a informação de controlo FTP? (In what connection is transported the FTP control information?)
São abertas duas conexões TCP pela aplicação FTP. A conexão que é aberta em primeiro lugar tem como propósito enviar os comandos FTP ao servidor e receber as respostas a esses mesmos comandos (informação de controlo), enquanto que a segunda recebe os dados enviados pelo servidor

## Quais são as fases de uma conexão TCP? (What are the phases of a TCP connection?)
Uma conexão TCP tem três fases: o estabelecimento da conexão, a troca de dados/informação e o encerramento da conexão.