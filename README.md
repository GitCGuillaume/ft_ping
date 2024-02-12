# ft_ping

Ce projet a besoin des droits administrateurs, il est basé sur le pack [inetutils](https://www.gnu.org/software/inetutils/) (version 2.0).

Ping est une commande qui permet de vérifier l'accessibilité vers une machine dans un réseau, pour ce faire, elle utilise le protocole de communication ICMP. ([Internet Control Message Protocol](https://fr.wikipedia.org/wiki/Internet_Control_Message_Protocol))

Commandes pour compiler ping:
```
make all
make bonus
```

```
Pour lancer la commande, utiliser sudo ./ft_ping ou devenir administrateur avec su
sudo ./ft_ping google.com
sudo ./ft_ping_bonus google.com
```
Simple ping:

![localhost](https://github.com/GitCGuillaume/ft_ping/assets/34135668/ecaaec43-1bb0-448f-9430-03540bfd443b)

Time-to-live, nombre qui se décrémente avant d'être rejeté par un routeur:

![ttl](https://github.com/GitCGuillaume/ft_ping/assets/34135668/da62ca97-7598-45e0-92e7-a6c9270c7fb6)

Cible introuvable:

![unreach](https://github.com/GitCGuillaume/ft_ping/assets/34135668/385f428d-41f0-44eb-910a-b7226722bfe0)

![verbose](https://github.com/GitCGuillaume/ft_ping/assets/34135668/db747fd0-afd9-45e2-bf96-1ec765245455)

Verbose (ou -v) permet d'afficher plus d'informations, ici le paquet de base avant envoi est affiché.

## Liste d'options

-v ou --verbose (sudo ./ft_ping -v google.com)

>Affiche des statistiques en plus

-? ou --help (sudo ./ft_ping --help)

>Liste tous les options disponibles (Texte de base de la commande ping du pack inetutils)

--usage (sudo ./ft_ping --usage)

>Affiche la liste des commandes de la commande ping du pack inetutils

## Liste d'options bonus

--ttl (sudo ./ft_ping_bonus -v --ttl 1 google.com)

>Modifier la valeur de base Time-to-live lors de la transmission du paquet

-w ou --timeout (sudo ./ft_ping_bonus -w 1 google.com)

>S'arrête après X seconds

-l ou --preload (sudo ./ft_ping_bonus -l 5 google.com)

>Envoi le nombre de paquets aussi rapidement que possible

-p ou --pattern (sudo ./ft_ping_bonus -p 123 google.com)

>Modifie les valeurs du paquet, cela est utile pour diagnostiquer les problèmes dans le réseau
>![data](https://github.com/GitCGuillaume/ft_ping/assets/34135668/ce91e745-e1c5-4f60-a6a2-2555aaedb045)

-T ou --tos (sudo ./ft_ping_bonus T 1 google.com)
>Règle la transmission de priorité paquet

