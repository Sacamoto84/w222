/*---------------------------------------------------------------------
 * Вызывается прослушивателем событий DOMContentLoaded ниже; добавить
 * Раскрывающийся список версии документации чуть выше формы поиска в
 * верхняя часть левой (навигационной) панели.
 *---------------------------------------------------------------------*/

function add_version_selector() {
  return fetch("https://raw.githubusercontent.com/lvgl/docs_compiled/gh-pages/versionlist.txt")
    .then((res) => res.text())
    .then((text) => {
      const versionList = text.split("\n").filter((version) => version.trim().length > 0);
      const versionDropdown = document.getElementById("version_dropdown");
      versionDropdown.innerHTML = versionList
        .map((version) => {
          let versionName = "";
          if (version == "master") versionName = "Latest";
          else versionName = "v" + (version.indexOf(".") != -1 ? version : version + " (latest minor)");
          return `<option value="${version}">${versionName}</option>`;
        })
        .join("");

      // Подождите следующего кадра анимации, чтобы убедиться, что DOM обновлен.
      requestAnimationFrame(() => {
        const versionDropdownValue = window.location.pathname.split("/")[1];
        const isValidVersion = versionDropdownValue.match(/^([0-9]+\.[0-9]+|master)$/);
        versionDropdown.value = isValidVersion ? versionDropdownValue : "master";
      });

      versionDropdown.addEventListener("change", ver_sel);
    });
}

/*---------------------------------------------------------------------
 * Срабатывает, когда пользователь выбирает версию документации из раскрывающегося списка версий.
 *---------------------------------------------------------------------*/
function ver_sel() {
  var x = document.getElementById("version_dropdown").value;
  var new_url = window.location.protocol + "//" + window.location.host + "/" + x + "/";

  if (new_url.startsWith("http")) {
    window.location.href = new_url;
  }
}

/*---------------------------------------------------------------------
 * Once:  добавить раскрывающийся список версий документации прямо над формой поиска в
 * верхняя часть левой панели.
 *---------------------------------------------------------------------*/
document.addEventListener("DOMContentLoaded", (event) => {
  add_version_selector().then(() => {
    var value = window.location.pathname.split("/")[1];
    document.getElementById("version_dropdown").value = value;
  });
});

/*---------------------------------------------------------------------
 * Этот прослушиватель откладывает загрузку (медленно загружаемых) примеров до тех пор, пока они не будут
 * фактически отображается в окне просмотра браузера.
 *
 * Once:  для каждого элемента класса «lv-example» добавляет наблюдателя, который наблюдает
 * чтобы этот элемент появился в области просмотра браузера.  Когда это
 * это так, функция`onIntersection()`запускается, вызывая загрузку примера
 * если он еще этого не сделал или удалил, когда его пересечениеRatio <= 0.
 */
document.addEventListener("DOMContentLoaded", (event) => {
  function onIntersection(entries) {
    entries.forEach((entry) => {
      let currentlyLoaded = entry.target.getAttribute("data-is-loaded") == "true";
      let shouldBeLoaded = entry.intersectionRatio > 0;
      if (currentlyLoaded != shouldBeLoaded) {
        entry.target.setAttribute("data-is-loaded", shouldBeLoaded);
        if (shouldBeLoaded) {
          let iframe = document.createElement("iframe");
          iframe.src = entry.target.getAttribute("data-real-src");
          entry.target.appendChild(iframe);
        } else {
          let iframe = entry.target.querySelector("iframe");
          iframe.parentNode.removeChild(iframe);
        }
      }
    });
  }
  const config = {
    rootMargin: "600px 0px",
    threshold: 0.01,
  };
  let observer = new IntersectionObserver(onIntersection, config);
  document.querySelectorAll(".lv-example").forEach((iframe) => {
    observer.observe(iframe);
  });
});

document.addEventListener("DOMContentLoaded", (event) => {
  /*---------------------------------------------------------------------
   * Для страниц API:
   *
   * Каждый элемент документированного кода на страницах API имеет структуру, которая выглядит
   * как этот пример для функции:
   *
   * <dl class="cpp function_state_> // контейнер
   *     <dt id="_function_identifier_">функция подписи</dt> // подсветка кода прогресса с помощью дочерних элементов <span>
   *     <dd>документация</dd> // документация, изначально закрытая
   *     </dl>
   *
   * где`_state_`— «расширенный» или «нерасширенный».  Когда «нерасширенный»
   * класс присутствует, элемент <dd>, содержащий документацию, скрыт
   * авторCSSв`custom.css`.  Без присутствия этого класса (т.е. при «расширении»
   * вместо этого присутствует класс), элемент <dd> возвращается в свое нормальное состояние
   * быть видимым.
   *
   * Задача приведенного ниже кода заключается в следующем:
   * 1.  Добавьте «нерасширенный» класс ко всем элементам <dl class="cpp ...">...
   * 2.  ...кроме случаев, когда уURLбыла хэш-строка ("... #identifier_string")
   *     который соответствует атрибуту id непосредственного дочернего элемента <dt>, в котором
   *     если вместо этого добавляется «расширенный» класс.  (Обычно это указывает
   *     пользователь перешел туда, щелкнув элемент кода с гиперссылкой в
   *     одна из страниц документации.)
   * 3.  Добавьте кнопку открытия/закрытия <span class="lv-api-expansion-button" />
   *     элемент непосредственно перед каждым элементом кода, чтобы переключить его развернутое/нерасширенное
   *     класс.  (Значок направления, на котором можно щелкнуть, до того, как он установлен в `custom.css`
   *     на основе «расширенного» или «нерасширенного» класса элемента <dl>.)
   *---------------------------------------------------------------------*/
  document.querySelectorAll("dl.cpp").forEach((cppListing) => {
    const dt = cppListing.querySelector("dt");
    let exClass = "expanded";
    let unExClass = "unexpanded";
    let shouldBeExpanded = false;
    if (dt.id == document.location.hash.substring(1)) shouldBeExpanded = true;
    if (shouldBeExpanded) {
      cppListing.classList.add(exClass);

      /* Если `dt.id` предназначен для перечислителя, также разверните его родительский элемент.
       * Элемент <dl class="cpp enum unexpanded">. */
      let parentDlNode = cppListing.parentNode.parentNode;
      if (
           parentDlNode != null
        && parentDlNode.classList.contains("cpp")
        && parentDlNode.classList.contains("enum")
        && parentDlNode.classList.contains(unExClass)
        ) {
        parentDlNode.classList.remove(unExClass);
        parentDlNode.classList.add(exClass);
      }

      /* Пусть браузер прокрутит`cppListing`в поле зрения, если это еще не сделано. */
      cppListing.scrollIntoView();
    } else {
      cppListing.classList.add(unExClass);
    }
    const button = document.createElement("span");
    button.classList.add("lv-api-expansion-button");
    button.addEventListener("click", () => {
      cppListing.classList.toggle(unExClass);
      cppListing.classList.toggle(exClass);
    });

    dt.insertBefore(button, dt.firstChild);
  });

  /*---------------------------------------------------------------------
   * Создайте любой текущий пользовательский баннер в`banner.json`вверху каждой страницы.
   *---------------------------------------------------------------------
   * Пользовательские баннеры вставляются между этими двумя элементами вверху страницы.
  <a class="skip-to-content muted-link" href="#furo-main-content">Перейти к содержимому</a>

  <div class="lv-custom-banner-list">
    <p class="lv-custom-banner">
      <em>Важное</em> объявление!
    </p>
    <p class="lv-custom-banner">
      <em>Важное</em> объявление номер два!
    </p>
    <p class="lv-custom-banner">
      <em>Важное</em> объявление третье!
    </p>
  </div>

  <div класс="страница">
    ...содержание страницы...
   *---------------------------------------------------------------------*/
  let bannerJsonUrl = "https://lvgl.io/data/banner.json";
  let bannerContainerClass = "lv-custom-banner-list";
  let bannerClass = "lv-custom-banner";
  /* Note:  Свойство приоритета баннера может иметь только одно из следующих значений:
   * («самый высокий» | «высокий» | «нормальный» | «низкий» | «самый низкий»).
   * Если его нет, по умолчанию используется «нормальный приоритет».  Это управляет стилем баннера. */
  let priorityPropVals = ["highest", "high", "normal", "low", "lowest"];
  let defaultPrioPropVal = priorityPropVals[2];
  let priorityClassSuffix = "-priority";

  /* Сортировка json-баннеров в порядке приоритета.
   * `a` и`b`— это объектыBANNERиз входящего`banner.json`. */
  function prio_compare(a, b) {
    let aPrioPropStr = a.hasOwnProperty("priority") ? a.priority : defaultPrioPropVal;
    let bPrioPropStr = b.hasOwnProperty("priority") ? b.priority : defaultPrioPropVal;
    let aPrio = 0;
    let bPrio = 0;

    /* Установите числовые значения для `a` и `b`. */
    for (var i = 0; i < 5; i++) {
      if (aPrioPropStr === priorityPropVals[i]) {
        aPrio = i;
        break;
      }
    }

    for (var i = 0; i < 5; i++) {
      if (bPrioPropStr === priorityPropVals[i]) {
        bPrio = i;
        break;
      }
    }

    /* Доказательство правильности
     * -----------------
     * < 0 = `a` должен идти раньше `b`.
     * > 0 = `a` должен идти после `b`.
     *   0 или NaN = а === б.
     *
     * Example:  а === "наивысший приоритет"; b === «нормальный приоритет».
     *           аПрио === 0 ; бПрио === 2.
     *           aPrio - bPrio === -2 означает (`a`должен идти раньше`b`).
     */
    return aPrio - bPrio;
  }

  fetch(bannerJsonUrl)
    .then((response) => {
      if (response.ok) {
        return response.json();
      } else {
        /* Note:  согласноOOSC2нецелесообразно создавать исключение для
         * ситуация, по которой проводится проверка.  Иногда файл баннера
         * там не будет, и в этом случае мы просто возвращаем пустой объект массива. */
        return [];
      }
    })
    /* Файл JSON успешно получен.... */
    .then((json) => {
      if (json.constructor !== Array) {
        /* Структура данных не распознана. */
      } else {
        /* console.log('JSON— массив.'); */
        /* Does it contain any banners? */
        if (json.length === 0) {
          console.log("JSON has no banners -- nothing to do.");
        } else {
          /* Note:  `div.page` уникален для темы Furo. */

          /* Создайте и вставьте контейнер баннеров. */
          const newDiv = document.createElement("div");
          newDiv.classList.add(bannerContainerClass);
          let bannerCount = 0;

          /* Создайте элемент <p> или <a> для каждого баннера.
           * Сначала отсортируйте их в порядке приоритета, при этом «наивысший приоритет» будет
           * наверху.  JSON — это ARRAY объектов BANNER.
           * `prio_compare()` умеет их сравнивать.
           *
           * Если объектBANNERимеет свойство «url», то
           * инкапсулировать баннер в элементе привязки, который будет отправлять
           * пользователю назначен URL.
           */
          json.sort(prio_compare);

          for (var i = 0; i < json.length; i++) {
            let banner = json[i];

            if (banner.hasOwnProperty("label")) {
              bannerCount++;
              let priorityClass = "";
              let newElement = null;

              if (banner.hasOwnProperty("url")) {
                newElement = document.createElement("a");
                newElement.setAttribute("href", banner.url);
              } else {
                newElement = document.createElement("p");
              }

              if (banner.hasOwnProperty("priority")) {
                priorityClass = banner.priority + priorityClassSuffix;
              } else {
                priorityClass = defaultPrioPropVal + priorityClassSuffix;
              }

              newElement.innerHTML = banner.label;
              newElement.classList.add(bannerClass);
              newElement.classList.add(priorityClass);
              newDiv.appendChild(newElement);
            }
          }

          if (bannerCount > 0) {
            const page = document.querySelector("div.page");
            const pgParent = page.parentElement;
            pgParent.insertBefore(newDiv, page);

            /* Наконец, нам нужно сообщить элементу страницы, что его `min-hight`
             * составляет 100% минус высота всех баннеров, включая один
             * предоставлено`conf.py`в `conf.html_theme_options.announcement`
             * если он присутствует === var(--header-height).
             *
             * Это удлинит короткие страницы ровно настолько, чтобы разместить [ PREV ] и [ NEXT ]
             * кнопки и нижний колонтитул внизу страницы без прокрутки.
             *
             * Note:  это переопределяет набор свойств`min-height`для этого
             * элемент в`furo.css`, имеющий вид: Calc(100% - var(--header-height)).
             * Дополнительно уменьшается высота списка баннеров.
             * */
            let height = newDiv.offsetHeight;
            page.style["min-height"] = `calc(100% - var(--header-height) - ${height}px)`;
          }
        }
      }
    })
    .catch((error) => {
      console.error("Fetch error: " + error.message);
    });
});

/*---------------------------------------------------------------------
 * Этот прослушиватель добавляет обработчик кликов ко всем элементам .toggle.
 * При нажатии на .toggle все дополнительные элементы, кроме .header, раскрываются.
 * При нажатии на .header все дочерние элементы переключаются.
 *---------------------------------------------------------------------*/
document.addEventListener("DOMContentLoaded", function () {
  document.querySelectorAll(".toggle").forEach(function (toggle) {
    Array.from(toggle.children).forEach(function (child) {
      if (!child.classList.contains("header")) {
        child.style.display = "none";
      }
    });
    var header = toggle.querySelector(".header");
    if (header) {
      header.addEventListener("click", function (e) {
        const toggle = e.target.closest(".toggle");
        Array.from(toggle.children).forEach(function (child) {
          if (!child.classList.contains("header")) {
            if (child.style.display === "none") {
              child.style.display = "";
            } else {
              child.style.display = "none";
            }
          }
        });
        header.classList.toggle("open");
      });
    }
  });
});
