{% import "common_macros.inc.c" as util -%}

{#/****** write structs and typedefs ******/#}
{% macro render_tagdef(name, tag, fx) %}
// *********************************************************
// item-collection {{name}}
{% if tag|count > 1 -%}
typedef struct {{name}}_{{fx}} {
    {{name}}_{{fx}}( const cnc_tag_t {{tag|join("_=-1, const cnc_tag_t ")}}_=-1 ) {{": "}}
    {%- for arg in tag -%}
    {{arg ~ "( " ~ arg ~ "_ )" ~ (", " if not loop.last)}}
    {%- endfor -%}
    {{" {}"}}
    cnc_tag_t {{tag|join(", ")}};
    bool operator==(const {{name}}_{{fx}} & o) const {
        return {% for arg in tag -%}
        {{arg}} == o.{{arg}}{{" && " if not loop.last}}
        {%- endfor -%}
        ;
    }
} {{name}}_{{fx}}_t;

template <> struct cnc_hash< {{name}}_{{fx}}_t >
{ size_t operator()(const {{name}}_{{fx}}_t& tt) const {
    return (
    {%- for arg in tag -%}
        {{" (tt." ~ arg ~ (")" if loop.first else " << ((3*" ~ loop.index ~ ")-1))") ~ (" +" if not loop.last)}}
    {%- endfor -%}
    {{" );"}}
} };

#ifdef _DIST_
CNC_BITWISE_SERIALIZABLE( {{name}}_{{fx}}_t );
#endif
{% else -%}
typedef cnc_tag_t {{name}}_{{fx}}_t;
{% endif -%}
{% endmacro %}

{#/****** write all puts for given step/env ******/#}
{% macro render_dependence(g, ctxt, datadep, getput='put', instantiate=False, todo=True) %}
    {%- if instantiate == False or getput or datadep.kind == 'ITEM' -%}
    {%- set tagkey = ("key" if  datadep.kind == 'ITEM' else 'tag') -%}
    {%- set isarange = [] -%}
    {%- for k in (datadep.key if datadep.kind == 'ITEM' else datadep.tag) -%}
    {% do isarange.append(1) if k.isRanged %}
    {%- endfor -%}
    {%- if datadep.kind == 'ITEM' and instantiate and isarange|count > 0 -%}
    {{g.itemDeclarations[datadep.collName].type ~ " " ~ datadep.binding ~ "_handle" ~ (" /* = TODO: array handle? */" if todo) ~ ";"}}
    {% endif -%}
    {%- for k in (datadep.key if datadep.kind == 'ITEM' else datadep.tag) -%}
    {%- if k.isRanged -%}
    {%- set idx = "i" ~ loop.index0 -%}
    {%- call util.render_indented(loop.index0 - 1) %}
    for ( int {{idx}} = {{k.start}}; {{idx}} < {{k.end}}; ++{{idx}} )
    {%- endcall -%}
    {%- endif -%}
    {%- endfor -%}
    {{" {" if isarange|count > 0 and datadep.kind == 'ITEM'}}
    {%- call util.render_indented(isarange|count) -%}
    {%- if datadep.kind == 'ITEM' and instantiate %}
    {{(g.itemDeclarations[datadep.collName].type) ~ " " ~ datadep.binding ~ "_handle" ~ (" /* = TODO: computation/initialization */" if todo and getput != 'get') ~ ";"}}
    {% endif -%}
    {%- if getput %}
    {{ctxt ~ datadep.collName ~ "." ~ getput ~ "( " ~ datadep.collName}}_{{tagkey}}_t( 
    {%- for k in (datadep.key if datadep.kind == 'ITEM' else datadep.tag) -%}
    {%- if k.isRanged -%}
    {{"i" ~ loop.index0 ~ (", " if not loop.last)}}
    {%- else -%}
    {{k.expr ~ (", " if not loop.last)}}
    {%- endif -%}
    {%- endfor -%}
    {{" )"}}
    {%- if datadep.kind == 'ITEM' -%}
    , {{datadep.binding}}_handle
    {%- endif -%}
    {{" );"}}
    {%- if todo and datadep.kind == 'ITEM' -%}
    {{" /* TODO: use it */" if getput == 'get' else (" /* TODO: array handle? */" if isarange|count > 0)}}
    {%- endif -%}
    {%- endif -%}
    {%- endcall %}
    {%- call util.render_indented((isarange|count)-1) %}
    {{"}" if isarange|count > 0 and datadep.kind == 'ITEM'}}
    {%- endcall -%}
    {%- endif -%}
{%- endmacro %}


{#/****** initialize/instantiate context members/vars ******/#}
{% macro render_ctxtargs_init(g, pfx='_', types=False) %}
   {%- for line in g.ctxParams -%}
   {%- set l = line.translate(None,',;') -%}
   {{line.split()[0] ~ " " if types == True else ", "}}
   {%- for p in l.split()[1:] -%}
   {{", " if not loop.first}}{{p}}( {{pfx}}{{p}} )
   {%- endfor -%}
   {{";" if types == True}}
   {% endfor %}
{%- endmacro %}


{#/****** write context arguments/parameters ******/#}
{% macro render_ctxtargs_param(g, types=True, pfx='_', sep=', ', dflt='') %}
   {%- for line in g.ctxParams -%}
   {%- set l = line.translate(None,',;') -%}
   {%- for p in l.split()[1:] -%}
   {{line.split()[0] ~ " " if types == True}}{{pfx}}{{p}}{{dflt}}{{(", " if not loop.last)}}
   {%- endfor -%}
   {{sep if not loop.last}}
   {%- endfor -%}
{%- endmacro %}
    {# {%- if isarange|count > 0 -%}
       [/* TODO */]
       {%- endif -%} #}
