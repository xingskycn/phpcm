PHP Cache Manager
=================

PHP Cache Mangaer - это менеджер кеша, созданный для упращения работы с кешированием в сложных, структурированных и высоконагруженных проектах.
В качестве backend для кеша используется memcached, в качестве интерфейса управления - расширение php.

# Features

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

    thread_1_day --- thread_1_day --- thread_1_month --- thread_1_all

Когда будет сброшен счетчик thread_1_day, счетчик thread_1_day тоже будет сброшен. Сброс счетчика thread_1_all сбрасывает все счетчики по цепочке.

Вот такой код мы выполняем при добавлении новго сообщения:

    $mc->set("thread_1_all", $newCountAll);

Т.к. новое количество будет на 1 больше, чем старое - это изменить значнеие ключа thread_1_day. Его изменение приведет к сбросу зависимых от него ключей (всех трех оставшихся по цепочке).

