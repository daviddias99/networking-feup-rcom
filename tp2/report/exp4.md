- How to configure a static route in a commercial router?
- What are the paths followed by the packets in the experiments carried out and why?
- How to configure NAT in a commercial router?
- What does NAT do?

### Experiência 4 - Configurar um router comercial e implementar NAT

Na quarta experiência foi introduzido no sistema um router comercial da Cisco (Rc). Os objetivos da experiência são configurar Rc, implementar no mesmo a técnica de Network Address Translation (NAT) e acomodar os restantes computadores (tux61 , tux62, tux64) à existência desse router.

A introdução de Rc é um ponto de partida para conferir aos computadores do sistema acesso à internet. Para configurar o router comercial foi necessário definir os endereços IP de ambas as suas portas, uma porta que serve de interface para a "rede interna" (endereço 172.16.61.254/24) e uma porta que serve de interface para a "rede externa" (endereço 172.16.1.69/24) a qual está ligada a rede do laboratório (e à internet). Os comandos de configuração são introduzidos através da consola do router ligada a um computador através da porta série:

```
interface gigabitethernet 0/0
ip address 172.16.61.254 255.255.255.0
no shutdown
...
interface gigabitethernet 0/1
ip address 172.16.1.69 255.255.255.0
no shutdown
...
```

O segundo passo é o de adicionar aos computadores tux61, tux62 e tux64 as rotas default. O router default de tux61 é tux64, e Rc é o router default de tux62 e tux64:

```
// In tux61
route add default gw 172.16.60.254

// In tux62 and tux64
route add default gw 172.16.61.254
```

Para além disso foi necessário adicionar a Rc a rota para a rede 172.16.60.0. Isto pode ser feito adicionando uma rota estática para a rede na consola de Rc:

```
ip route 172.16.60.0 255.255.255.0 172.16.61.253
```

O comando usado define que para a gama de endereços definido pelo par endereço+mascara dados como primeiro e segundo argumento, o endereço do próximo "hop" deverá ser o endereço dado pelo terceiro argumento.

Através de vários comandos de ping foi possível verificar que todas as interfaces (de tux62, tux64 e ambas as interfaces de Rc) estavam atingíveis a partir de tux61. Também foi possível verificar que a rede 172.16.60.0 estava atingível a partir de tux62. Assim confirmou-se que todas as rotas estavam bem definidas.

Ao retirar a rota para a rede 172.16.60.0 e desligando o redirecionamento de ICMP em tux62, efetuando um traceroute para tux61 verificou-se que os pacotes enviados seguiam primeiro para Rc (172.16.61.254) e de seguida para tux64. Isto acontece porque Rc é o default gateway de tux62 e Rc possui uma rota para a rede 172.16.60.0 através de tux64. Através dos logs foi possível observar a chegada de mensagens de ICMP redirect provenientes de Rc.

Ativando o redirecionamento de ICMP em tux62 e efetuando traceroute novamente foi possível observar que, mesmo sem possuir a rota para a rede 172.16.60.0, tux62 conseguiu "aprender" que tux64 seria uma melhor rota praa a rede 172.16.60.0.

Ao efetuar um ping de tux61 para o router do laboratório e analisando os logs, verificou-se que o router não estava atingível. Isto deve-se ao facto de, dado a Rc não ter a funcionalidade de NAT implementada, os pacotes ping request chegam ao router do laboratório com um endereço de origem  da rede interna (neste caso 172.16.60.1) e, portanto, este router não tem conhecimendo de para onde deve enviar os pacotes da resposta (não possui nenhuma rota para a "rede interna").


Para adicionar a funcionalidade de Network Address Translation ao router Cisco, são utilizados os seguintes comandos na consola de Rc:

```
...

// Definir a porta 0 do router com IP 172.16.61.254/24 e declará-la 
// como a porta "interior" do NAT (porta que contacta com a rede interna)
interface gigabitethernet 0/0
ip address 172.16.61.254 255.255.255.0
no shutdown
ip nat inside
...

// Definir a porta 1 do router com IP 172.16.1.69/24 e declará-la 
// como a porta "exterior" do NAT (porta que contacta com a rede externa/internet)
interface gigabitethernet 0/1
ip address 172.16.1.69 255.255.255.0
no shutdown
ip nat outside
...
// Definir uma "pool" de endereços globais com o nome de "ovrld" que podem 
// ser alocados pelo NAT
ip nat pool ovrld 172.16.1.69 172.16.1.69 prefix 24

// Definir as gamas de endereços que têm permissão serem traduzidos pelo NAT.
// Neste caso endereços como 172.16.61.253 (tux64), estão fora desta gama 
// e portanto não podem usufruir do NAT (não têm acesso à internet).
ip nat inside source list 1 pool ovrld overload
access-list 1 permit 172.16.60.0 0.0.0.7
access-list 1 permit 172.16.61.0 0.0.0.7

// Definir a default gateway do router e adicionar uma rota para a rede 172.16.60.0
ip route 0.0.0.0 0.0.0.0 172.16.1.254
ip route 172.16.60.0 255.255.255.0 172.16.61.253
...
```

A adição de NAT em Rc permite a tradução dos endereços da rede interna (rede constituida pelos computadores tux6X) no endereço exterior do router Cisco, o qual está mapeado no router do laboratório que eventualmente tem "ligação à internet". Deste modo é possível a  máquinas exteriores à rede receber e enviar pacotes para estes computadores.
Para qualquer pacote com endereço de origem na rede interna (salvo endereços exlcuidos pela configuração do NAT), Rc irá traduzir o endereço IP num par composto pelo seu "endereço global" (172.16.1.69) mais o número da porta alocada a esse endereço interno, enviando o pacote "modificado" para o seu destino. Inversamente, à chegada de um pacote com o IP de destino 172.16.1.69 e um número de uma porta, RC traduz este par num endereço da rede interna, enviando o pacote pela rota configurada.
