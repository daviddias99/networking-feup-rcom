- How to configure vlany0?
- How many broadcast domains are there? How can you conclude it from the logs?

### Experiência 2 - Implementar duas LANS virtuais (VLANs) num *switch*

Na segunda experiência foi introduzido um novo computador (**tux62**) o qual foi configurado de uma forma semelhante à vista na experiência 1. A novidade nesta experiência é a criação (e configuração) de duas VLANs numa das quais estão conectados os computadores **tux61** e **tux64** e na outra o computador **tux62**. O objetivo da experiência é o de estudar as ligações entre os vários computadores associados a VLANs.

A configuração de **tux62** foi feita com recurso ao comando _**ifconfig**_ tendo sido atribuido ao computador o endereço de IP 172.16.61.1 (escolhido por nós) e o endereço de MAC 00:21:5a:5a:7d:9c (característico da carta *ethernet*).

O processo de criação das VLANs foi efetuado através do computador **tux61** que estava conectado com o *switch* da *Cisco* através de uma ligação na porta série. Para criar as duas VLANs foi utilizado o comando "**vlan** **id**", tendo sido criada uma VLAN com id=60 e uma VLAN com id=61. Daqui em diante, iremo-nos referir a estas VLANS como **vlan60** e **vlan61** respetivamente. Criadas as VLANs foi necessário adicionar as portas do switch onde estavam conectados os três computadores às VLANS corretas. Estando o computador **tux61** ligado à porta **#1**, **tux62** ligado à porta **#2** e tux64 ligado à porta **#3**, foi necessário adicionar a porta **#1** e **#3** à **vlan60** e a porta **#2** à **vlan61**. para o efeito foram utilizados os seguintes comandos:

```
...
interface fastethernet 0/X
switchport mode access
switchport acces vlan 6Z
...
```

Em que **X** representa a porta coorrespondente e **Z** o último digito da VLAN pretendida. Por exemplo, para adicionar **tux61** à **vlan60**, X=1 e Z=0, visto **tux61** estar ligado à porta #1 o switch e esta pertencer à **vlan60**.

Efetuando um ping a partir de tux61 para o IP de tux64 e tux62 foi possível ver que apenas o ping para tux64 (172.16.60.254) obteve resposta. De igual modo, efetuando um ping broadcast a partir de **tux61** para o enderereço 172.16.60.255 (comando *ping -b 172.16.60.255*) foi possível observar que apenas em **tux64** se detetava a chegada de pacotes **ICMP**. Ao efetuar um ping broadcast a partir de **tux62** para o endereço  172.16.61.255, não foi recebido nenhum pacote de **ICMP** nos restantes dois computadores.

Os resultados das experiências efetuadas vieram confirmar que as VLANs foram configuradas corretamente pois foi possível observar que embora os três computadores estivessem ligados ao mesmo *switch*, não existia uma conecção entre os computadores **tux61/tux64** e o computador **tux62** dado a se encontrarem em VLANs distnintas, existindo portanto dois domínios de broadcast. Com isto é possível perceber a utilidade do uso de VLANs permitindo a criação de sub-redes lógicas numa rede de área local física.
