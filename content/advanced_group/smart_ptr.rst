Умные указатели (для продвинутой группы по C++)
###############################################

:date: 2020-04-09 00:00
:summary: Умные указатели
:status: draft

.. default-role:: code
.. contents:: Содержание

Ресурсы
=======

Ресурсы и владение
------------------

Концепция ресурсов и владения является одной из важнейших в C++. 

Обычно считается, что ресурс - это нечто внешнее по отношению к программе. Например, ресурсом являются системные дескрипторы открытых файлов, сокеты, блокировки (mutex-ы) и так далее. В качестве ресурса можно рассматривать и выделенную динамическую память.

В ходе выполнения программы, ресурс сначала выделяется (получается), затем используется и, наконец, освобождается. Корректное освобождение ресурсов, в первую очередь, важно для приложений, которые предназначены для длительного выполнения, однако пренебрежение освобождением ресурсов является плохой практикой. Не смотря на то, что в большинстве случаев после завершения приложения операционная система самостоятельно освобождает запрошенные им ресурсы, часто такое освобождение является нетривиальным. Например, если ресурсом является соединение с базой данных через сокет, то, скорее всего, это соединение не будет автоматически закрыто сразу, а лишь спустя определенное время, когда операционная система пометит это соединение как "повисшее" (stale) и закроет его самостоятельно. Проблема значительно усугубляется, если приложение активно использует и запрашивает ресурсы, но не освобождает их когда они перестают быть нужны.

RAII
----

В объектно-ориентированном приложении за работу с ресурсами обычно отвечают объекты, которые предоставляют остальным компонентам приложения возможность работать с ресурсом через свои методы вместо использования системных или библиотечных функций напрямую. Такой способ работы является правильным как с точки зрения проектирования приложения, так и с точки зрения убдоства работы программиста.

Экземпляр класса, который хранит ссылку на ресурс, является владельцем этого ресурса. Прочие части приложения не владеют ресурсом напрямую, а работают с ним через владельца. Это позволяет переложить ответственность за выделение и освобождение ресурса только на владельца. У одного и того же ресурса может быть несколько владельцев, и в этом случае вопрос освобождения ресурса при уничтожении одного из владельцев все еще является ответственностью владельца.

В хорошо спроектированном приложении, когда выполняется принцип единственной ответственности (single responsibility), отвечающий за работу с ресурсом класс не имеет других задач, кроме работы с этим ресурсом. В этом случае удобно использовать идиому RAII (Resourse Aquision Is Initializaton, получение ресурса есть инициализация).

Суть этой идиомы в следующем: инициализация нового экземпляра класса осуществляется посредством получения им ресурса, владельцем которого он становится. Распространенной практикой здесь является выделение этого ресурса при создании объекта (например захват блокировки при создании объекта-обертки). В процессе своего уничтожения, экземпляр должен освободить ресурс (например снять блокировку), если это возможно, и если он является единственным владельцем.

Идиома RAII позволяет гарантировать корректное освобождение всех ресурсов, в том числе в случаях, когда в ходе работы возникают исключения. Это следует из того, что даже при возникновении необработанного исключения, из-за которого происходит досрочный выход из функций, созданные в этих функциях локальные переменные уничтожаются, соответственно вызываются деструкторы объектов, в которых и происхоит освобождение ресурсов.

Ссылки
======

Указатели и lvalue-ссылки
-------------------------

Основным способом работы с памятью в C++ являютя указатели. Указатель - это переменная,  значением которой является адрес в памяти, по которому хранится значение. Указатель не обязательно хранит адрес выделенной в куче области памяти, он также может указывать на локальную переменную, область куда загружен программный код, или на любую другую ячейку памяти.

Когда возникает необходимость передать объект в функцию не копируя его, можно просто передать указатель на него. Это хороший явный способ, который позволяет явно видеть что происходит и с чем мы работаем, однако зачастую он приводит к появлению в программе нагромаждений операторов ``*``, ``->`` и ``&``.

Для удобства разработчика в стандарте С++ существуют ссылки (reference). При передаче объекта по ссылке копирование не выполняется, вместо этого функция, которой передается ссылка, получает возможность работать с этим объектом так же, как и с любой другой переменной в своей области видимости. Ссылки не следует считать каким-то принципиально отличным от указателей механизмом передачи аргументов и работы с ними, на самом деле всю работу выполняет компилятор, и программа продолжает работать с указателями.

Ряд руководств по оформлению кода предписывают не использовать ссылки для передачи в функции изменяемых параметров, поскольку, по мнению авторов этих руководств, это затрудняет чтение кода. В частности, google C++ style guide предлагает передавать по константным ссылкам параметры, которые используются только для чтения, и использовать указатели для передачи изменяемых параметров, например: ``void Foo(const std::string &in, std::string *out)`` . Автор этого текста не видит причин не прислушаться к этой рекомендации, если она не противоречит стандартам оформления кода, принятым в проекте, с которым вы работаете.

Перемещение и rvalue ссылки
---------------------------

На практике иногда возникают ситуации, когда используется перемещение объекта. Простейшим примером здесь может служить обмен значений двух переменных с использованием промежуточной. В случае "наивной" реализации, при таком обмене будет три раза выполнено копирование объектов, которое является "дорогой" операцией, не всегда нужно, а иногда просто невозможно. Для таких ситуаций существуют rvalue-ссылки, которые являютя стандартным для С++ способом реализации семантики перемещения (move semantics).

Значение, которое передается в функцию по rvalue-ссылке, эта функция может использвать как угодно, считается что это значение больше не нужно там, откуда оно передано. Опять же, следует понимать, что все это является "примечанием" для разработчика, и должно использоваться для облегчения написания и чтения кода. Типовым примером использования rvalue-ссылок является написание для объекта конструктора, принимающего на вход rvalue-ссылку и выполняющего передачу ресурсов от экземпляра-аргумента к создаваемому экземпляру.

Правило пяти
------------

У классов можно выделить несколько "особых" методов:
 1. Конструктор копирования по константной ссылке (``Class(const Class& rhs)``)
 2. Оператор присвоения по константной ссылке (``Class& operator=(const Class& rhs)``)
 3. Конструктор перемещения (``Class(Class&& rhs)``)
 4. Оператор присвоения перемещением (``Class& operator=(Class&& rhs)``)
 5. Деструктор (``~Class()``)

Правило пяти гласит, что обычно в классах, которые реализуют хотя бы один из этих методов, следует реализовать все пять.

Сужением этого правила является правило трех, которое гласит то же самое, но только для пунктов 1, 2 и 5 этого списка, и применяется в случаях когда семантика перемещения для класса сознательно не реализуется разработчиком. Еще одним дополнением является правило четырех (двух), которое гласит, что если все ресурсы, которыми пользуется экземпляр класса, находятся во владении других классов, использующих идиому RAII, то деструктор обычно можно не реализовывать.

Сжатие ссылок и perfect forwarding
----------------------------------

Зачем это нужно
***************

Стандарт C++ не допускает использования ссылок на ссылки. Это кажется нелогичным, но позволяет значительно упростить чтение кода, поскольку, в отличии от указателей, ссылки разработчики обычно рассматривают как "обычный" объект, не задумываясь о том, как происходит разадресация при обращении.

Представим себе, что нам нужно реализовать обертку, которая будет принимать на вход два параметра и передавать их в какую-то другую функцию (такая задача возникает, например, при написании классов-оберток). Рассмотрим несколько вариантов того, как это может быть реализовано. Начнем с самого простого:

.. code :: c++

  template <class T, class U>
  void wrapper1(T a1, U a2) {
    internal(a1, a2);
  }

Этот вариант имеет существенный недостаток: при его использовании будет выполнено копирование аргументов во временные объекты, после чего в функцию будут переданы уже копии. Безусловно, это нежелательно. Попробуем это исправить:

.. code :: c++

  template <class T, class U>
  void wrapper2(T& a1, U& a2) {
    internal(a1, a2);
  }

Этот вариант выглядит лучше, но у него есть неочевидная проблема. Поскольку здесь используются не-константные ссылки, то при такой реализации не скомпилируется вполне тривиальный код: ``wrapper2(1, 2.0)``. Это произойдет потому, что в качестве аргументов мы передаем rvalue (*не путать с rvalue-ссылками!*), а взять не-константную ссылку на него нельзя.

.. code :: c++

  template <class T, class U>
  void wrapper3(const T& a1, const U& a2) {
    internal(a1, a2);
  }

Такой вариант почти хорош, но и здесь могут возникнуть проблемы. Возникнут они в том случае, например, когда функция ``internal`` ожидает получить на вход не-константные ссылки. Можно решить эту проблему, перегрузив функцию ``wrapper`` 4 раза (для всех комбинаций константных и не-константных ссылок), однако это решение не является хорошим, так как число перегрузок будет расти с числом аргументов экспоненциально.

Сжатие ссылок
*************

Для решения этой проблемы следует вернуться к тому, как описаны ссылки в стандарте C++. Рассмотрим следующий пример кода:

.. code :: c++

  template <class T>
  void fun(T x) {
    T& ref = x;
  }
  
  int main() {
    int64_t i;
    fun<int64_t&>(i);
  }

Поскольку мы явно обозначили как раскрывать шаблон, то компилятор будет следовать нашим указаниям. При этом переменная ref будет иметь тип ``int64_t& &``. Стандарт C++, начиная с C++11, предписывает компилятору в этом случае вывести тип этой переменной как ``int64_t&``.  Таким образом происходит "сжатие ссылок" (reference collapsing). Правила, по которым оно выполняется, следующие:

  1. ``T& & => T&``
  2. ``T&& & => T&``
  3. ``T& && => T&``
  4. ``T&& && => T&&``

По сути, можно упростить их до следующего: lvalue-ссылка всегда остается lvalue-ссылкой.

Forwarding reference
********************

Стандарт C++ содержит в себе еще одну особенность: правила вывода шаблонных типов в конструкциях следующего вида:

.. code :: c++

  template <class T>
  void fun(T&& x) {
    // ...
  }
  
Не смотря на то, что синтаксически T&& выглядит как rvalue-ссылка, в действительности это не так. Эта конструкция называется forwarding reference (ранее называлась universal reference). Шаблонный тип будет раскрыт по-разному в зависимости от того, что было передано в функцию. Если в функцию передано lvalue (например, переменная) типа ``U``, то он будет выведен как ``U&``. Если же было передано rvalue типа ``U`` (см. пример после wrapper2), то тип будет выведен как ``U``. Это правило кажется странным, однако можно видеть, что оно позволяет решить задачу написания обертки так:

.. code :: c++

  template <class T, class U>
  void wrapper4(T&& a1, U&& a2) {
    internal(a1, a2);
  }

На самом деле не совсем. Правильным решением будет:

.. code :: c++

  template <class T, class U>
  void wrapper5(T&& a1, U&& a2) {
    internal(std::forward<T>(a1), std::forward<U>(a2));
  }

Здесь использование std::forward позволяет сохранить тип ссылки при передаче аргумента.

**Замечание:** Perfect forwarding и принцип его работы являются сложной темой. Здесь приведен лишь краткий пересказ основных фактов. В конце этого материала можно найти ссылки для более подробного изучения.

Умные указатели
---------------

Умные указатели являются реализацией RAII для управления динамически выделяемой памятью. Их использование позволяет предотвращать утечки памяти без значительных изменений в коде. Существует два основных вида умных указателей, которые будут рассмотрены отдельно.

Unique ptr
**********

Unique ptr (уникальный указатель) - это реализация умного указателя с единственным владельцем ресурса. При создании нового unique_ptr, этот экземпляр сохраняет "обычный" указатель на область памяти, которой он теперь владеет, и предполагает, что она используется только им. Когда unique_ptr уничтожается, он освобождает выделенную память.

Для обеспечения единоличного владения ресурсом, unique_ptr запрещено копировать. С точки зрения реализации, это означает, что конструктор копирования и оператор присвоения с копированием для него удалены. При этом его можно перемещать. При перемещении, новый указатель "забирает" себе ресурс. Это реализуется тривиально - новый unique_ptr сохраняет значение обычного указателя, которое хранилось в старом, а значение в старом заменяет на ``nullptr``. Таким образом, старый unique_ptr перестает быть связан с этой памятью.

Shared ptr
**********

Не всегда удобно работать с памятью при помощи указателя, который нельзя скопировать. В этих случаях на помощь приходит shared_ptr - умный указатель со счетчиком ссылок. Так же, как и unique_ptr, shared_ptr хранит внутри указатель на память, а также указатель на блок ссылок, который можно упрощенно представить так:

.. code :: c++

  struct RefCntBlock {
    size_t strong_refs, weak_refs;
  };

Здесь ``strong_refs`` и ``weak_refs`` - это счетчики, которые показывают, сколько shared_ptr и weak_ptr ссылаются на память по указателю. В деструкторе shared_ptr уменьшает счетчик ``strong_refs`` и, если он равен нулю, освобождает память. Соответственно, в ходе копирования новый shared_ptr сохраняет себе указатели на память и блок счетчиков, но не затирает их у старого. Перемещение shared_ptr выполняется так же, как и в случае unique_ptr, но перемещается не только указатель на память, а и указатель на блок счетчиков.

Weak ptr
********

При использовании shared_ptr может возникнуть проблема циклических ссылок. Пусть у нас есть код такого типа:

.. code :: c++

  struct Human {
    std::shared_ptr<Human> neighbour;
  };

  void fun() {
    std::shared_ptr<Human> first(new Human()), second(new Human());
    second.neighbour = first;
    first.neighbour = second;
  }

Поскольку эти две структуры ссылаются друг на друга, количество ссылок на выделенную память не равно нулю ни для одной из структур, и память очищена не будет, не смотря на то, что shared_ptr на эти структуры, созданные в функции fun, были уничтожены при выходе из нее. Проблема циклических ссылок хорошо известна в языках программирования со сборщиком мусора (garbage collector), которые зачастую также используют указатели со счетчиками ссылок.

В качестве решения этой проблемы в C++ были добавлены "слабые" указатели - weak_ptr. Они аналогичны shared_ptr, отличия заключаются в том, что их количество учитывается в счетчике weak_refs, и они не владеют памятью, хоть и хранят указатель на нее.

Для weak_ptr-ов определена операция "материализации", посредством которой из weak_ptr-а получается shared_ptr, являющийся владельцем памяти. Эта операция возможна только если память еще не была совобождена. В случаях, когда память уже была освобождена, weak_ptr считается "истекшим" (expired). Из свойств shared_ptr и weak_ptr следует, что равенство счетчика ссылок strong_refs нулю является необходимым и достаточным условием того, что память была освобождена. При этом weak_ptr является одним из владельцев памяти, выделенной под блок счетчиков, поэтому освобождение этой памяти происходит только когда оба счетчика в блоке равны нулю.

make_unique и make_shared
*************************

Помимо самих unique_ptr и shared_ptr, стандарт C++ описывает шаблонные функции ``make_unique<T, ...Args>`` и ``make_shared<T, ...Args>``. Они принимают на вход аргументы произвольных типов (Args), конструируют новый экземпляр класса T, передавая в качестве параметров конструктора эти аргументы, и возвращают соответственно unique_ptr или shared_ptr, владеющие этим экземпляром. На практике, в большинстве случаев, использование этих функций предпочтительнее, нежели выделение памяти с использованием **new** и передача указателя на эту память в конструкторы умных указателей. Помимо того, что make_unique и make_shared позволяют более явно указать, что работа с памятью напрямую не происходит и происходить не должна, make_shared выделяет место не только под создаваемый объект, но и под блок счетчиков, что позволяет хранить данные более оптимально и обойтись меньшим количеством обращений к аллокатору.

Безопасность
************

При разработке многопоточных приложений следует аккуратно использовать умные указатели. Не смотря на то, что эти указатели сами по себе потокобезопасны (например, блок счетчиков в shared_ptr, на самом деле, хранит счетчики в виде атомарных целочисленных значений), они никак не защищают объекты, на которые ссылаются. Помимо того, операции, которые касаются самих умных указателей (копирование, присвоение и т.д.) следует защищать блокировками самостоятельно.

Также следует избегать конструкций вида:

.. code :: c++

  void fun(std::shared_ptr<int64_t> p, int64_t q) { /* ... */}
  int64_t go() { return 2; }
  int main() {
    fun(std::shared_ptr<int64_t>(new int64_t(1)), go());
  }

Это связано с тем, что стандарт C++ не гарантирует порядок вычисления аргументов функции. Он гарантирует только то, что все аргументы будут вычислены до того, как произойдет вызов функции. В данном случае компилятор имеет полное право скомпилировать код так, что сначала будет выполнен ``new int64_t(1)``, затем ``go()`` и лишь потом будет вызван конструктор ``shared_ptr``. Тогда, если в функции ``go()`` возникнет исключение, то память, выделенная ``new int64_t(1)``,  не будет освобождена, поскольку ``shared_ptr`` еще не стал ее владельцем.

При использовании shared_ptr как указателя на экземпляры собственного класса, часто удобно объявить этот класс наследником класса ``std::enable_shared_from_this`` для того, чтобы иметь возможность порождать новые shared_ptr на экземпляры. Не следует напрямую создавать shared_ptr используя this в качестве аргумента, так как в данном случае никак не обрабатывается ситуцация, когда на экземпляр класса уже есть shared_ptr, что может привести к двойному освобождению выделенной под этот экземпляр памяти.

Также необходимо соблюдать осторожность при захвате shared_ptr в лямбда-функции, это может привести к возникновению циклических ссылок.

Задание
-------

Архив с заданием находится по ссылке: https://mega.nz/file/H7h3gADK#b9dk_kZ-jQnfQsDMj5HAv22ZbZjceC5K4vyz7baxgqw. 

Вам необходимо посмотреть, какие методы есть у классов unique_ptr, shared_ptr и weak_ptr в стандартной библиотеке, и самостоятельно реализовать некоторые из них, следуя предоставленному интерфейсу. Править необходимо только файл sptr.h (при желании можно создать дополнительные файлы). Сборка для проверки будет осуществляться аналогично деку:

.. code ::

  g++ -std=c++17 -fsanitize=address,undefined main.cpp -c
  g++ -std=c++17 -fsanitize=address,undefined main.o sptr.cpp

Ссылки
------

  1. https://en.cppreference.com/w/cpp/language/raii
  2. https://habr.com/ru/post/226229/
  3. https://habr.com/ru/post/348198/
  4. https://en.cppreference.com/w/cpp/language/rule_of_three
  5. https://ru.cppreference.com/w/cpp/memory/unique_ptr
  6. https://ru.cppreference.com/w/cpp/memory/shared_ptr
  7. https://ru.cppreference.com/w/cpp/memory/enable_shared_from_this
  8. https://en.cppreference.com/w/cpp/utility/forward
  9. https://habr.com/ru/post/191018/
  10. https://habr.com/ru/post/471326/
  11. https://google.github.io/styleguide/cppguide.html#Reference_Arguments
