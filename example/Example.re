module P ={
  let defer = f : Js.Promise.t('a) =>
  Js.Promise.make((~resolve, ~reject as _) => ignore(f(resolve)));

let return = (a: 'a) : Js.Promise.t('a) => Js.Promise.resolve(a);

let error = (a: exn) : Js.Promise.t('a) => Js.Promise.reject(a);

let (>>=) = (m: Js.Promise.t('a), f: 'a => Js.Promise.t('b)) =>
  Js.Promise.then_(f, m);
let pfunc = (>>=);
/** Same as >>= with auto return */
let (>>-) = (m: Js.Promise.t('a), f: 'a => 'b) =>
  Js.Promise.then_(a => return(f(a)), m);

let (>>|) = (m: Js.Promise.t('a), f: Js.Promise.error => Js.Promise.t('a)) =>
  Js.Promise.catch(f, m);

let (>>/) = (m: Js.Promise.t('a), f: Js.Promise.error => 'a) => Js.Promise.catch(e => return(f(e)), m);
};

let (>>=) =P.pfunc;
let my_func = resolve =>
  Js.Global.setTimeout(() => resolve(. 9999), 2000);
let _ =
  P.defer(my_func)
  >>= (x => P.return(123 + x))
  >>= (y => P.return(Js.log2("test my_func", y)));
// open Prom;
// let thingThatTakesACallback =(resolve)=>Js.Global.setTimeout(() => [@bs] resolve("Done!"), 1000)


//   // open Prom;
// let thingThatReturnsAPromise = () => {
//   let (p, resolve) = Prom.make();
//   thingThatTakesACallback(thingWeWaitedFor=>resolve(thingWeWaitedFor));
//   p;
// }
open Prom;
let thingThatTakesACallback =()=>
  Js.Promise.make(
    (~resolve, ~reject as _) => {
      ignore(Js.Global.setTimeout(() => [@bs] resolve("Done!"), 1000));
      ()
    }
  );


// let thingThatReturnsAPromise = () => {
//   let (p, resolve) = make();
//   thingThatTakesACallback(thingWeWaitedFor => {
//     resolve(thingWeWaitedFor)
//   });
//   p;
// }
