import 'mocha';
import { expect } from 'chai';
const Evaluator = require('/var/www/LORIS/Loris/jsx/lib/CAPParser/js/Evaluator');
describe('CAPParser Unit Tests', () => {
  describe('When passed an empty string', () => {
    it('Throws an error', () => {
      expect(() => Evaluator('')).to.throw();
    })
  })

  describe('When passed null', () => {
    it('Returns null', () => {
      const LOGIC_STR = 'null';
      const res = Evaluator(LOGIC_STR);
      expect(res).to.equal(null);
    })
  })

  describe('Simple equation', () => {
    it('Evaluates the equation, maintaining order of ops', () => {
      const LOGIC_STR = 'abs((sqrt(64)-12^(4^(1/2)))/(min(5,99,104.1232234,3.0001,3))+1/3)'; 
      const res = Evaluator(LOGIC_STR);
      expect(res).to.equal(45);
    })
  })

  describe('Simple if statement', () => {
    it('Evaluates the equation, maintaining order of ops', () => {
      const LOGIC_STR = 'if(true, 2 + " string test", 0)';
      const res = Evaluator(LOGIC_STR);
      expect(res).to.equal("2 string test");
    })
  })

  describe('Complex equation containing nested ifs, boolean operations, counting operations, and string concatenation', () => {
    it('Evaluates the equation, maintaining order of ops and context', () => {
      const LOGIC_STR = 'max(if([d]="North America",if([e]<>"Montreal",1,0.5),0),if(([a]+1)/[c]>(product(100,1/2,1/2,1/2,1/2,1/(12.5))),2,0)) + ", " + [e] + ", " + [d]'; 
      const CONTEXT = {a: 100, b: 50, c: 1, d: "North America", e: "Montreal"}; 
      const res = Evaluator(LOGIC_STR, CONTEXT);
      expect(res).to.equal("2, Montreal, North America");
    })
  })
  
  describe('Complex boolean operation', () => {
    it('evals', () => {
      const LOGIC_STR = 'if((5+4)>=[a] and not ((4>10) or [d]),5,4)';
      const CONTEXT = {a: 9, d: false};
      const res = Evaluator(LOGIC_STR, CONTEXT);
      expect(res).to.equal(5);
    })
  })

  describe('Rounding operations', () => {
    it('evals', () => {
      const LOGIC_STR = 'if(rounddown([a],[b])<>roundup([a],[b]) and (round([a],[b])<>roundup([a],[b]) or round([a],[b])<>rounddown([a],[b])),round([a],[b]),"Rounding failure")';
      const CONTEXT = {a: 2.43298570129128437893429384, b: 4};
      const res = Evaluator(LOGIC_STR, CONTEXT);
      expect(res).to.equal(2.4330);
    })
  })
  
  describe('Rounding operations', () => {
    it('evals', () => {
      const LOGIC_STR = 'round([a],[b])';
      const CONTEXT = {a: 2.4, b: 4};
      const res = Evaluator(LOGIC_STR, CONTEXT);
      expect(res).to.equal(2.4000);
    })
  })

  describe('Date difference operations', () => {
    it('evals', () => {
      const LOGIC_STR = 'datediff("2017-01-01","2017-01-01T17:30:00Z","h","ymd", 0)';
      const CONTEXT = {a: 2, b: 4};
      const res = Evaluator(LOGIC_STR, CONTEXT);
      expect(res).to.equal(17.5);
    })
  });
})
