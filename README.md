# PhysX WASM Workflow @WEBAVERSE

## Introduction ##
PhysX is a C++ open-source realtime physics engine middleware SDK developed by Nvidia.
Webaverse has adopted it in WebAssembly, which is compiled using Emscripten.

## Setup Requirements

### Get [Webaverse PhysX Build](https://github.com/webaverse/app-wasm) (APP-WASM)
- Fork and clone it to your prefered directory.


### Get [Emscripten SDK](https://github.com/emscripten-core/emsdk)
- For it to work with the current build you need to install version **2.0.6**
- Clone it and then run these commands inside the **emsdk** directory:
    `./emsdk install 2.0.6`
    `./emsdk activate 2.0.6`
    

## Compilation

- Within **app-wasm** point to the **emsdk** directory using this command:
`source /home/user/emsdk/emsdk_env.sh` change it to your own path.

- Now inside **app-wasm** run this to compile:
`./compile.sh`

- Congrats! If it compiled without errors the finished build should be in `app-wasm/bin`

## Use it in Webaverse

- Copy the `geometry.js` and `geometry.wasm` files from `app-wasm/bin`
- Paste them into `app/public/bin` in the Webaverse app
- Open `geometry.js` in a code editor and *find* **"geometry.wasm"** then replace it with **"bin/geometry.wasm"**
![geometry.js suffix](https://i.ibb.co/3yYjYm1/geom.png)

- Add this snippet at the end of `geometry.js`:

```javascript=2
let accept, reject;
const p = new Promise((a, r) => {
  accept = a;
  reject = r;
});
Module.postRun = () => {
  accept();
};
Module.waitForLoad = () => p;
run();
export default Module;
```
![geometry.js suffix](https://i.ibb.co/W3Kv29g/geometry.png)


- Done! Now reload the Webaverse app and try out your own PhysX configuration!

---

## PhysX Docs

- https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Index.html
- https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/Index.html

