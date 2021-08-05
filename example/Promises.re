/**
 * Making promises
 */
let okPromise = Js.Promise.make((~resolve, ~reject as _) => [@bs] resolve("ok"));

/* Simpler promise creation for static values */
Js.Promise.resolve("easy");

Js.Promise.reject(Invalid_argument("too easy"));

/* Create a promise that resolves much later */
let timer =
  Js.Promise.make(
    (~resolve, ~reject as _) => {
      ignore(Js.Global.setTimeout(() => [@bs] resolve("Done!"), 1000));
      ()
    }
  );


/**
 * Handling promise values
 * Note that we *have* to return a new promise inside of the callback given to then_;
 */
 Js.log("Handling promise values")
Js.Promise.then_((value) => Js.Promise.resolve(Js.log(value)), okPromise);
 Js.log("----------------------")
/* Chaining */
 Js.log("Chaining")
Js.Promise.then_(
  (value) => Js.Promise.resolve(Js.log(value)),
  Js.Promise.then_((value) => Js.Promise.resolve(value + 1), Js.Promise.resolve(1))
);
 Js.log("----------------------")
/* Better with pipes 😉 */
Js.Promise.resolve(1)
|> Js.Promise.then_((value) => Js.Promise.resolve(value + 1))
|> Js.Promise.then_((value) => Js.Promise.resolve(Js.log(value)));

/* And even better with some Reason spice */
Js.Promise.(
  resolve(1)
  |> then_((value) => resolve(value + 1))
  |> then_((value) => resolve(Js.log(value)))
);

/* Waiting for two values */
Js.Promise.(
  all2((resolve(1), resolve("a")))
  |> then_(
       ((v1, v2)) => {
         Js.log("Value 1: " ++ string_of_int(v1));
         Js.log("Value 2: " ++ v2);
         resolve()
       }
     )
);

/* Waiting for an array of values */
Js.Promise.(
  all([|resolve(1), resolve(2), resolve(3)|])
  |> then_(
       ([|v1, v2, v3|]) => {
         Js.log("Value 1: " ++ string_of_int(v1));
         Js.log("Value 2: " ++ string_of_int(v2));
         Js.log("Value 3: " ++ string_of_int(v3));
         resolve()
       }
     )
);


/**
 * Error handling
 */
/* Using a built-in OCaml error */
let notFoundPromise = Js.Promise.make((~resolve as _, ~reject) => [@bs] reject(Not_found));

Js.Promise.then_((value) => Js.Promise.resolve(Js.log(value)), notFoundPromise)
|> Js.Promise.catch((err) => Js.Promise.resolve(Js.log(err)));

/* Using a custom error */
exception Oh_no(string);

let ohNoPromise = Js.Promise.make((~resolve as _, ~reject) => [@bs] reject(Oh_no("oh no")));

Js.Promise.catch((err) => Js.Promise.resolve(Js.log(err)), ohNoPromise);


/**
 * Unfortunately, as one can see - catch expects a very generic `Js.Promise.error` value
 * that doesn't give us much to do with.
 * In general, we should not rely on rejecting/catching errors for control flow;
 * it's much better to use a `result` type instead.
 */
let betterOk =
  Js.Promise.make((~resolve, ~reject as _) => [@bs] resolve(Js.Result.Ok("everything's fine")));

let betterOhNo =
  Js.Promise.make((~resolve, ~reject as _) => [@bs] resolve(Js.Result.Error("nope nope nope")));

let handleResult =
  Js.Promise.then_(
    (result) =>
      Js.Promise.resolve(
        switch result {
        | Js.Result.Ok(text) => Js.log("OK: " ++ text)
        | Js.Result.Error(reason) => Js.log("Oh no: " ++ reason)
        }
      )
  );

handleResult(betterOk);

handleResult(betterOhNo);


/**
 * "Better living through functions."
 * This section is for collecting useful helper functions when handling promises
 */
/* Get rid of the need for returning a promise every time we use `then_` */
let thenResolve = (fn) => Js.Promise.then_((value) => Js.Promise.resolve(fn(value)));

Js.Promise.(resolve(1) |> thenResolve((value) => value + 1) |> thenResolve(Js.log));

/* Get rid of pesky compiler warnings at the end of a side-effectful promise chain */
let thenIgnore = (fn, p) => thenResolve((value) => fn(value), p) |> ignore;
