{% import "common_macros.inc.c" as util -%}
{#/****** write all puts for given step/env ******/#}
{% macro render_dependence(g, datadep, getput='put', instantiate=False, todo=True) %}
    {%- if instantiate == False or getput or datadep.kind == 'ITEM' -%}
    {%- set isarange = [] -%}
    {%- for k in (datadep.key if datadep.kind == 'ITEM' else datadep.tag) -%}
    {%- if k.isRanged -%}
    {% do isarange.append(1) %}
    {%- if datadep.kind == 'ITEM' -%}
    {%- set idx = "i" ~ loop.index0 -%}
    {% call util.render_indented(loop.index0) %}
    for ( int {{idx}} = {{k.start}}; {{idx}} < {{k.end}}; ++{{idx}} )
    {%- endcall -%}
    {%- endif -%}
    {% endif -%}
    {%- endfor -%}
    {{" {" if isarange|count > 0 and datadep.kind == 'ITEM'}}
    {%- call util.render_indented(isarange|count) -%}
    {%- if datadep.kind == 'ITEM' and instantiate %}
    {{(g.itemDeclarations[datadep.collName].type ~ datadep.binding ~ "_handle" ~ (" = /* TODO: computation/initialization */" if todo and getput != 'get') ~ ";")}}
    {% else %}
    {% endif -%}
    {% if getput -%}
    {{datadep.collName ~ "." ~ getput ~ "( " ~ datadep.collName ~ "_key_t( "}}
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
    {{" );" ~ (" /* TODO: use it */" if todo and getput == 'get')}}
    {%- endif -%}
    {%- endcall %}
    {%- call util.render_indented((isarange|count)-1) %}
    {{"}" if isarange|count > 0 and datadep.kind == 'ITEM'}}
    {%- endcall -%}
    {%- endif -%}
{%- endmacro %}

{% macro render_ctxtargs_init(g, sfx='_') %}
   {%- for line in g.ctxParams -%}
   {%- set l = line.translate(None,',;') -%}
   {%- for p in l.split()[1:] -%}
   , {{p}}( {{p}}{{sfx}} )
   {% endfor %}
   {%- endfor %}
{%- endmacro %}

{% macro render_ctxtargs_param(g, types=True, sfx='_', sep=', ') %}
   {%- for line in g.ctxParams -%}
   {%- set l = line.translate(None,',;') -%}
   {%- for p in l.split()[1:] -%}
   {{line.split()[0] ~ " " if types == True}}{{p}}{{sfx}}{{(", " if not loop.last)}}
   {%- endfor -%}
   {{sep if not loop.last}}
   {%- endfor -%}
{%- endmacro %}
    {# {%- if isarange|count > 0 -%}
       [/* TODO */]
       {%- endif -%} #}
