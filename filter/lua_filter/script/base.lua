-- by shajf

local ffi = require 'ffi'
local ffi_new = ffi.new
local error = error
local setmetatable = setmetatable
local floor = math.floor
local ceil = math.ceil


local str_buf_size = 4096
local str_buf
local size_ptr

local ok, new_tab = pcall(require, "table.new")
if not ok then
    new_tab = function (narr, nrec) return {} end
end


local ok, clear_tab = pcall(require, "table.clear")
if not ok then
    clear_tab = function (tab)
                    for k, _ in pairs(tab) do
                        tab[k] = nil
                    end
                end
end


do
    local orig_require = require
    local pkg_loaded = package.loaded
    local function my_require(name)
        local mod = pkg_loaded[name]
        if mod then
            return mod
        end
        return orig_require(name)
    end
    getfenv(0).require = my_require
end

if not pcall(ffi.typeof, "ch_packet_t") then
    ffi.cdef[[
        typedef struct {}ch_packet_t;
    ]]
end


if not pcall(ffi.typeof, "ch_ffi_str_t") then
    ffi.cdef[[
        typedef struct {
            int      len;
            char    *data;
        } ch_ffi_str_t;
    ]]
end


local c_buf_type = ffi.typeof("char[?]")


local _M = new_tab(0, 16)


_M.new_tab = new_tab
_M.clear_tab = clear_tab


local errmsg


function _M.get_errmsg_ptr()
    if not errmsg then
        errmsg = ffi_new("char *[1]")
    end
    return errmsg
end


function _M.set_string_buf_size(size)
    if size <= 0 then
        return
    end
    if str_buf then
        str_buf = nil
    end
    str_buf_size = ceil(size)
end


function _M.get_string_buf_size()
    return str_buf_size
end


function _M.get_size_ptr()
    if not size_ptr then
        size_ptr = ffi_new("size_t[1]")
    end

    return size_ptr
end


function _M.get_string_buf(size, must_alloc)
    
    if size > str_buf_size or must_alloc then
        return ffi_new(c_buf_type, size)
    end

    if not str_buf then
        str_buf = ffi_new(c_buf_type, str_buf_size)
    end

    return str_buf
end

_M.FFI_OK = 0
_M.FFI_ERROR = -1

return _M
