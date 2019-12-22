- What are the ARP packets and what are they used for?
- What are the MAC and IP addresses of ARP packets and why?
- What packets does the ping command generate?
- What are the MAC and IP addresses of the ping packets?
- How to determine if a receiving Ethernet frame is ARP, IP, ICMP?
- How to determine the length of a receiving frame?
- What is the loopback interface and why is it important?

### Experiência 1 - Configurar uma rede IP (não sei se está bem traduzido)
A primeira experiência consistiu em configurar os endereços IP e tabelas de roteamento de dois computadores de modo a permitir comunicação entre eles.

Os endereços IP das interfaces eth0 de ambos os computadores foram configurados utilizando o comando _**ifconfig**_. O computador **tux61** ficou com endereço IP 127.16.60.1 e endereço MAC 00:0f:fe:8c:af:71 e o computador **tux64** ficou com endereço IP 127.16.60.254 e endereço MAC 00:21:5a:c5:61:bb. Após a configuração, verificou-se, através do comando *__ping__* que os computadores estavam conectados. Não foi necessário configurar rotas, pois ambos os computadores estavam na mesma LAN. Na imagem abaixo encontram-se as tabelas de encaminhamento(onde se pode verificar, no **tux61**, que o **gateway** para a rede 172.16.60.0, que é a rede onde se encontram os computadores, é 0.0.0.0, ou seja, a rede está ligada diretamente à interface) e de ARP (onde, no **tux61** se pode verificar qual o endereço físico correspondente ao IP do computador **tux64**, e vice-versa).

Após limpar a entrada da tabela ARP(caso isto não fosse feito o o tux61 não enviaria um pacote ARP, pois teria o endereço MAC do tux64 na tabela ARP), utilizando o programa wireshark na interface eth0 do tux61 para capturar os pacotes enviados ao fazer o comando ping do tux61 para o tux64, verificou-se que o tux61 começou por enviar por broadcast um pacote ARP(bytes 12 e 13 do cabeçalho da trama ethernet iguais a 0x0806), com o objetivo de obter o endereço MAC associado ao IP da interface eth0 do tux64. Os endereços IP e MAC de origem deste pacote correspondem aos do tux61, o endereço MAC de destino é 00:00:00:00:00:00, pois o endereço do computador de destino é desconhecido, e o IP de destino corresponde ao tux64. A resposta foi um pacote ARP com endereços IP e MAC de origem e de destino correspondentes aos tux64 e tux61, respetivamente, a informar qual o endereço MAC do tux64.

Conhecendo agora o endereço MAC do tux64, os pacotes gerados pelo comando ping são pacotes IP(bytes 12 e 13 do cabeçalho da trama ethernet iguais a 0x0800) protocolo ICMP(byte 23 igual a 0x01), com endereços MAC e IP de origem correspondentes ao da máquina que envia o pacote e de destino correspondentes ao da máquina para o qual se envia o pacote.

A interface loopback é uma interface de rede virtual qm que as mensagens enviadas para esta interface são recebidas pela máquina que as enviou. Esta interface pode ser útil para efeitos de teste, bem como para ser cliente de um servidor da própria máquina.

TODO: RESPONDER À PERGUNTA DO SIZE