// Copyright (c) 2013-15, LKC Technologies, Inc.
// Все права защищены.
//
// Распространение и использование в исходной и двоичной форме, с или без
// Модификация допускается при соблюдении следующих условий:
//
// При повторном распространении исходного кода должно сохраняться указанное выше уведомление об авторских правах.
// список условий и следующий отказ от ответственности. Перераспределения в двоичном формате
// форма должна воспроизводить вышеуказанное уведомление об авторских правах, настоящий список условий и
// следующий отказ от ответственности в документации и/или других материалах
// предоставляется вместе с раздачей.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef CODE128_H
#define CODE128_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Поскольку символы FNCn не являются ASCII , определите здесь версии, чтобы
// упростить строки кодирования, которые их включают.
#define CODE128_FNC1 '\xf1'
#define CODE128_FNC2 '\xf2'
#define CODE128_FNC3 '\xf3'
#define CODE128_FNC4 '\xf4'

size_t code128_estimate_len(const char * s);
size_t code128_encode_gs1(const char * s, char * out, size_t maxlength);
size_t code128_encode_raw(const char * s, char * out, size_t maxlength);

#ifdef __cplusplus
}
#endif

#endif // CODE128_H
