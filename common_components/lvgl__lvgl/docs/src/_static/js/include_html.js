
/*https://www.w3schools.com/howto/howto_html_include.asp*/
function includeHTML() {
  var z, i, elmnt, file, xhttp;
  /*перебрать коллекцию всех элементов HTML:*/
  z = document.getElementsByTagName("*");
  for (i = 0; i < z.length; i++) {
    elmnt = z[i];
    /*поиск элементов с определенным атрибутом:*/
    file = elmnt.getAttribute("include-html");
    if (file) {
      /*сделайте запрос HTTP, используя значение атрибута в качестве имени файла:*/
      xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4) {
          if (this.status == 200) {elmnt.innerHTML = this.responseText;}
          if (this.status == 404) {elmnt.innerHTML = "Page not found.";}
          /*удалите атрибут и вызовите эту функцию еще раз:*/
          elmnt.removeAttribute("w3-include-html");
          includeHTML();
        }
      }
      xhttp.open("GET", file, true);
      xhttp.send();
      /*выйти из функции:*/
      return;
    }
  }
};