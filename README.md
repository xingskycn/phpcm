PHP Cache Manager
=================

PHP Cache Mangaer - это менеджер кеша, созданный для упрощения работы с кешированием в сложных, структурированных и высоконагруженных проектах.
В качестве backend для кеша используется memcached, в качестве интерфейса управления - расширение php.

# Features

Реализованные возможности:

1. Стандартные возможности memcached клиента (get/set/delete/add/mget/flush). Шардинг кеша, flush отдельных шардов из php.
2. Dependency Cache - система зависимостей для сброса кеша.
3. Cache Replication - репликация кеша, для его отказоустойчивости
4. Cache Debug - инструмент для удобного Debug-а кеша.
5. ReShard - возможность "мягкого" добавления новых нод memcached, без потери 1/n данных кеша.

## ReShard

ReShard - возможность "мягкого" добавления новых нод memcached, без потери 1/n данных кеша. Пользоватся этой возможностью очень просто. Вот такая у нас была конфигурация кеша:

    $mc = new Cm([
       [ 'host' => '192.168.3.29', 'port'=>11211 ],
       [ 'host' => '192.168.3.30', 'port'=>11211 ],
    ]);

Мы хотим добавить новый сервер memcached (192.168.3.31) без потери 1/n данных кеша. Нам нужно изменить на некоторое время конфигурацию так:

    $mc = new Cm([
       [ 'host' => '192.168.3.29', 'port'=>11211 ],
       [ 'host' => '192.168.3.30', 'port'=>11211 ],
       [ 'newhost' => '192.168.3.31', 'port'=>11211 ],
    ]);

PHP Cacahe Manager, видя вхождение 'newhost' начинает работать по следующему принципу - он читает данные (get, mget) только со старой схемы (...3.29, ...3.30), а записы выполняет и по старой схеме (...3.29, ...3.30) и по новой схеме (...3.39, ...3.30, ...3.31). Спустя некоторое время, мы увидим, например, с помощью мониторинга, что новый сервер memcached заполнился данными. После этого можно изменять конфигурацию:

    $mc = new Cm([
       [ 'host' => '192.168.3.29', 'port'=>11211 ],
       [ 'host' => '192.168.3.30', 'port'=>11211 ],
       [ 'host' => '192.168.3.31', 'port'=>11211 ],
    ]);

Теперь кеш работает по новой схеме, при этом, благодаря тому что кеш записался и по старой схеме и по новой - все данные кеша доступны. После этого произойдет пик вытиснений из memcached (3.29 и 3.30) - это вытяснятся старые ключи, хранимые по старой схеме и больше не используемые (они теперь лежат на сервере 3.31)

## Cache Replication

Cache Replication - реализация отказоустойчивого кеша с помощью репликации. Репликация настраивается в Runtime в конструкторе объекта класса Cm. Рассказать проще на примере.

Пример простого кеша без репликации:

    $mc = new Cm([ ['host'=>'127.0.0.1', 'port'=>11211] ]);

Пример разделенного между двумя memcached-серверами кеша без репликации:

    $mc = new Cm([ 
      [ 'host'=>'127.0.0.1', 'port'=>11211 ], //ShardA
      [ 'host'=>'127.0.0.1', 'port'=>11212 ], //ShardB
     ]);

Пример разделенного кеша с репликацией:

    $mc = new Cm(
      [ //shardA
        [ 'host'=>'192.168.1.29', 'port'=>11211 ], //ShardA, replica 1
        [ 'host'=>'192.168.2.29', 'port'=>11211 ], //ShardA, replica 2
      ],
      [ //shardB
        [ 'host'=>'192.168.1.30', 'port'=>11211 ], //ShardB, replica 1
        [ 'host'=>'192.168.2.30', 'port'=>11211 ], //ShardB, replica 2
        [ 'host'=>'192.168.3.30', 'port'=>11211 ], //ShardB, replica 3
      ]
    );

Не обязательно, чтобы все шарды были реплицированы:

    $mc = new Cm([
       [ 'host'=>'192.168.1.29', 'port'=>11211 ], //ShardA
       [
         [ 'host'=>'192.168.1.30', 'port'=>11211 ], //ShardB, replica 1
         [ 'host'=>'192.168.2.30', 'port'=>11211 ], //ShardB, replica 2
       ]
    ]);

Репликация одного шарда:

    $mc = new Cm([
      [   //single Shard
         [ 'host'=>'192.168.1.30', 'port'=>11211 ], //single Shard, replica 1
         [ 'host'=>'192.168.2.30', 'port'=>11211 ], //single Shard, replica 2
      ]
    ]);

Просытм конструированием объекта можно легко сделать отказоустойчивый кеш.

## Cache Debug

Cache Debug - возможность записывать подробную информацию в syslog о том, что происходит с кешем. Для того, чтобы Cache Debug заработал
нужно собрать PHP Cache Manager со специальной опцией (см. wiki) и включить Debug при конструировании объекта Cm():

    $enableCacheBool = true;
    $cm = new Cm($configuration, $enableCacheBool);

Когда Debug включен в syslog начнут поступать сообщения следующего вида:

    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: Debug output started
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: delete: key=mkeyA
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: delete: result=success
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: delete: key=mkeyB
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: delete: result=success
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: set: key=mkeyA
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: set: result=success
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: set: key=mkeyB
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: set: result=success
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: get: key=mkeyA
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: get: result=success/isset
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: get: key=mkeyB
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: get: result=success/isset
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: connection: closing
    Nov  4 11:09:09 lynx php: (21594:140467582330688) [ info  ] PHPCM: connection: closed

Формат лога зависит от конкретной настройки syslog, но все, начиная сразу после "php:" зависит от PHP CM и описано ниже:

* (21594:140467582330688) - Pid процесса и pthread_self номер потока
* [ info ] - тип записи в лог. Может быть info, warning или error.
* PHPCM: - имя модуля который пишет лог, константа "PHPCM:"
* set: - действие, которое делаем модуль.
* key=mkeyA - описание, что происходит внутри действия.

Мы рекомендуем включать Cache Debug на dev-/test- окружениях и не рекомендуем включать его на Production средах, т.к. логирование в syslog
заметдляет работу приложения. Логирование идет в LOG_LEVEL6 с priority INFO.


## Dependency Cache

Dependency Cache - возможность сброса большого количества ключей, через изменение одного ключа-зависимости. Ключ-зависимость, как и любой
другой ключ может содержать в себе полезные данные.

**Пример задачи**. Допустим у нас есть счетчики сообщений в ветке форума - "всего", "за месяц", "за день", "за 3 часа".
Эти счетчики закешированы по ключам "thread_1_all", "thread_1_month", "thread_1_day", "thread_1_3hour".
При добавлении сообщения в ветку форума, нам нужно сбрасывать все эти кеши.

**Пример использования**. С использованием Dependency Cache можно решить эту задачу просто. При расчете значений счетчиков (на моменте отображения страницы),
при условии отсутсвия их в кеше выполняется следующее:

    $mc->set("thread_1_all", $countAll);                                      //кеш общего счетчика "невечно"
    $mc->set("thread_1_month", $countMonth, "thread_1_all", 3600*24);         //кеш счетчика за месяц "на день", т.к. при наступлении следующего дня счетчик стоит пересчитать
    $mc->set("thread_1_day", $countDay, "thread_1_month", $diffToEndOfDay);   //кеш счетчика за день "до конца суток", т.к. при наступлении 00:00 счетчик стоит пересчитать
    $mc->set("thread_1_3hour", $count3hour, "thread_1_day", 3600);            //кеш счетчика за 3 чеса "на час", т.к. при наступелении следующего часа счетчик стоит обновить

Выше мы сохранили счетчики и выставили expire на случай, если новые сообщения добавляться не будутm но данные в кеше будут устаревать со временем. Так же мы сделали цепочку зависимостей:

    thread_1_3hour --- thread_1_day --- thread_1_month --- thread_1_all

Когда будет сброшен счетчик thread_1_day, счетчик thread_1_3hour тоже будет сброшен. Сброс счетчика thread_1_all сбрасывает все счетчики по цепочке.

Вот такой код мы выполняем при добавлении новго сообщения:

    $mc->set("thread_1_all", $newCountAll);

Т.к. новое количество будет на 1 больше, чем старое - это изменить значнеие ключа thread_1_day. Его изменение приведет к сбросу зависимых от него ключей (всех трех оставшихся по цепочке).

# Next Features

1. Dependency Purge - возможность сброса кеша по сложной схеме зависимостей


